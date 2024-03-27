#pragma once
#include "Common/pch.h"

enum class ScheduleStrategy
{
    FIFO,
    LIFO,
    PRIORITY
};

enum class JobType
{
    RENDER,
    PHYSICS,
    AUDIO,
    NETWORK,
    AI,
    FILE_IO,
    MEMORY,
    ALLOCATOR,
    DEALLOCATOR,
    CUSTOM
};

class Job;
class Worker;
class JobPipeLine;
class JobSystem;

class Job
{
public:
    Job(std::function<void()> executeFunction, uint32_t workerID = UINT_MAX, uint8_t priority = 0)
        : m_ExecuteFunction(executeFunction),
          m_IsFinished(false),
          m_Priority(priority)
    {
    }

    void Execute()
    {
        if (m_ExecuteFunction)
        {
            m_ExecuteFunction();
        }
        m_IsFinished = true;
    }

    bool IsFinished()
    {
        return m_IsFinished;
    }

    template <class F, class... Args>
    auto BindOperation(F &&f, Args &&...args)
        -> std::future<typename std::invoke_result<F(Args...)>::type>
    {
        using return_type = typename std::invoke_result<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            m_ExecuteFunction = (*task)();
        }
        return res;
    }

    friend inline bool operator<(const Job &a, const Job &b)
    {
        return a.m_Priority < b.m_Priority;
    }

    friend inline bool operator<(const std::unique_ptr<Job> &a, const std::unique_ptr<Job> &b);

private:
    bool m_IsFinished = false;
    uint8_t m_Priority = 0;
    std::function<void()> m_ExecuteFunction;
};

inline bool operator<(const std::unique_ptr<Job> &a, const std::unique_ptr<Job> &b)
{
    return a->m_Priority < b->m_Priority;
};

class Worker
{
public:
    Worker() = delete;
    Worker(JobPipeLine* pipeLine);
    ~Worker();
    void Run();
    void Reset();
    bool IsBusy() const;
    bool IsInPipeLine() const;
    bool SetPipeLine(JobPipeLine* pipeLine);
private:
    void Stop();
    bool WakeUp();

private:
    std::atomic<bool> m_IsRunning;
    std::thread m_Thread;
    std::unique_ptr<Job> m_CurrentJob;
    JobPipeLine*m_PipeLine = nullptr;
    std::mutex *m_Mutex = nullptr;
    std::condition_variable *m_Condition = nullptr;
};

class JobPipeLine
{
public:
    JobPipeLine() = delete;
    JobPipeLine(uint32_t workerCount)
    {
        m_Workers.reserve(workerCount);
        for (uint32_t i = 0; i < workerCount; i++)
        {
            m_Workers.push_back(std::make_unique<Worker>(this));
        }
        HLOG_INFO("JobPipeLine created with %d workers\n", workerCount);
    }
    ~JobPipeLine() = default;
    JobPipeLine(const JobPipeLine &) = delete;
    JobPipeLine(JobPipeLine &&) = delete;
    JobPipeLine &operator=(const JobPipeLine &) = delete;

    void PushJob(std::unique_ptr<Job> &job)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        m_Jobs.push_back(std::move(job));
        m_JobCV.notify_one();
    }

    bool PopJob(std::unique_ptr<Job> &job)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        if (m_Jobs.empty())
        {
            m_JobCV.notify_all();
            return false;
        }
        job = std::move(m_Jobs.front());
        m_Jobs.pop_front();
        return true;
    }

    bool TransferInWorker(std::unique_ptr<Worker> &worker)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        worker->Reset();
        m_Workers.push_back(std::move(worker));
        return true;
    }

    bool TransferInWorkers(std::vector<std::unique_ptr<Worker>> &workers)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        if (workers.empty())
            return false;
        m_Workers.reserve(workers.size());
        for (auto &worker : workers)
        {
            worker->Reset();
            m_Workers.push_back(std::move(worker));
        }
        return true;
    }

    bool TransferOutWorker(std::unique_ptr<Worker> &worker)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        if (m_Workers.empty())
        {
            return false;
        }
        worker = std::move(m_Workers.back());
        m_Workers.pop_back();
        return true;
    }

    bool TransferOutAllWorkers(std::vector<std::unique_ptr<Worker>> &workers)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        if (m_Workers.empty())
        {
            return false;
        }
        workers = std::move(m_Workers);
        return true;
    }

    bool IsIdle()
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        return m_Jobs.empty();
    }

    void SortJobs(ScheduleStrategy strategy)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        switch (strategy)
        {
        case ScheduleStrategy::FIFO:
            break;
        case ScheduleStrategy::LIFO:
            std::reverse(m_Jobs.begin(), m_Jobs.end());
            break;
        case ScheduleStrategy::PRIORITY:
            std::sort(m_Jobs.begin(), m_Jobs.end());
            break;
        default:
            break;
        }
    }

    bool BorrowWorker(JobPipeLine *pipeLine)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        if (m_Workers.empty())
        {
            return false;
        }
        std::unique_ptr<Worker> worker;
        if (!TransferOutWorker(worker))
        {
            return false;
        }
        pipeLine->TransferInWorker(worker);
        m_PipeLinesBorrowed.push_back(pipeLine);
        return true;
    }

    bool CallReturnWorker(JobPipeLine *pipeLine)
    {
        std::lock_guard<std::mutex> guard(m_QueueMutex);
        if (m_PipeLinesBorrowed.empty())
        {
            return false;
        }
        std::vector<std::unique_ptr<Worker>> workers;
        if (!pipeLine->TransferOutAllWorkers(workers))
        {
            return false;
        }
        TransferInWorkers(workers);
        m_PipeLinesBorrowed.pop_back();
        return true;
    }

private:
    friend class Worker;
    std::deque<std::unique_ptr<Job>> m_Jobs;
    std::vector<std::unique_ptr<Worker>> m_Workers;
    std::vector<JobPipeLine *> m_PipeLinesBorrowed;
    std::mutex m_QueueMutex;
    std::mutex m_JobMutex;
    std::condition_variable m_JobCV;
};

class JobSystem
{
private:
    JobSystem();

public:
    ~JobSystem();
    uint32_t CreateNewPipeLine();
    bool DestroyPipeLine(uint32_t pipeLineID);
    void SortJobs(ScheduleStrategy strategy);
    bool SubmitJob(std::unique_ptr<Job> &job, const uint32_t pipeLineID = 0);

public:
    static JobSystem *CreateJobSystem();
    static void DestroyJobSystem(JobSystem *jobSystem);

private:
    uint32_t m_MaxNumOfWorkers = std::thread::hardware_concurrency();
    uint32_t m_CurrentNumOfWorkers = 0;
    std::vector<std::unique_ptr<JobPipeLine>> m_JobPipeLines;
    std::mutex m_Mutex;
};

inline bool GetJobSystem(JobSystem **jobSystem);

#ifdef MODULE_TEST
inline void TestJobSystem()
{
    std::unique_ptr<JobSystem> jobSystem(JobSystem::CreateJobSystem());
    for (int i = 0; i < 10000; i++)
    {
        std::string log = "Job " + std::to_string(i) + " executed\n";
        std::unique_ptr<Job> job = std::make_unique<Job>([log]()
                                                         { HLOG_INFO(log.c_str()); });
        jobSystem->SubmitJob(job);
    }
    jobSystem->SortJobs(ScheduleStrategy::PRIORITY);
}
#endif