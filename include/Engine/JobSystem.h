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
class Factory;
class JobSystem;

class Job
{
public:
    Job(std::function<void()> executeFunction, uint32_t workerID = UINT_MAX, uint8_t priority = 0)
        : m_ExecuteFunction(executeFunction),
          m_IsFinished(false),
          m_WorkerID(workerID),
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

    uint32_t GetWorkerID() const
    {
        return m_WorkerID;
    }

    friend inline bool operator<(const Job &a, const Job &b)
    {
        return a.m_Priority < b.m_Priority;
    }

    friend inline bool operator<(const std::unique_ptr<Job> &a, const std::unique_ptr<Job> &b);

private:
    uint32_t m_WorkerID = UINT_MAX;
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
    Worker(uint32_t workerID = UINT_MAX) : m_IsRunning(true), m_WorkerID(workerID)
    {
        m_Thread = std::thread(&Worker::Run, this);
    }

    ~Worker()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Condition.wait(lock, [&]
                         { return !m_IsRunning && m_CurrentJob == nullptr; });
        if (m_Thread.joinable())
        {
            m_Thread.join();
        }
    }

    void Run()
    {
        while (m_IsRunning)
        {
            {
                std::unique_lock<std::mutex> lock(m_Mutex);
                m_Condition.wait(lock, [this]
                                 { return !m_IsRunning || m_CurrentJob != nullptr; });
                if (!m_IsRunning)
                    break;
                m_CurrentJob->Execute();
                m_CurrentJob = nullptr;
            }
            Activate();
        }
    }

    void Activate()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        {
        }
        lock.unlock();
        if (m_CurrentJob != nullptr)
            WakeUp();
        else
            Stop();
    }

    bool IsBusy() const
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        return m_CurrentJob != nullptr;
    }

    bool IsDedicated() const
    {
        return m_WorkerID != UINT_MAX;
    }

    void SetWorkerID(uint32_t workerID)
    {
        m_WorkerID = workerID;
    }

    uint32_t GetWorkerID() const
    {
        return m_WorkerID;
    }

private:
    void Stop()
    {
        {
            std::lock_guard<std::mutex> guard(m_Mutex);
            m_IsRunning = false;
        }
        m_Condition.notify_all();
    }

    bool WakeUp()
    {
        {
            std::lock_guard<std::mutex> guard(m_Mutex);
            m_IsRunning = true;
        }
        m_Condition.notify_all();
        return true;
    }

    uint32_t m_WorkerID;
    mutable std::mutex m_Mutex;
    std::condition_variable m_Condition;
    std::atomic<bool> m_IsRunning;
    std::thread m_Thread;
    std::unique_ptr<Job> m_CurrentJob;
};

class JobPipeLine
{
public:
    JobPipeLine() = default;
    ~JobPipeLine() = default;
    JobPipeLine(const JobPipeLine &) = delete;
    JobPipeLine(JobPipeLine &&) = delete;
    JobPipeLine &operator=(const JobPipeLine &) = delete;

    void PushJob(std::unique_ptr<Job> &job)
    {
        std::lock_guard<std::mutex> guard(m_JobMutex);
        m_Jobs.push_back(std::move(job));
        m_JobCV.notify_one();
    }

    bool PopJob(std::unique_ptr<Job> &job)
    {
        std::lock_guard<std::mutex> guard(m_JobMutex);
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
        std::lock_guard<std::mutex> guard(m_JobMutex);
        m_Workers.push_back(std::move(worker));
        return true;
    }

    bool TransferInWorkers(std::vector<std::unique_ptr<Worker>> &workers)
    {
        std::lock_guard<std::mutex> guard(m_JobMutex);
        if (workers.empty())
            return false;
        m_Workers.reserve(workers.size());
        for (auto &worker : workers)
        {
            m_Workers.push_back(std::move(worker));
        }
        return true;
    }

    bool TransferOutWorker(std::unique_ptr<Worker> &worker)
    {
        std::lock_guard<std::mutex> guard(m_JobMutex);
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
        std::lock_guard<std::mutex> guard(m_JobMutex);
        if (m_Workers.empty())
        {
            return false;
        }
        workers = std::move(m_Workers);
        return true;
    }

    bool IsIdle()
    {
        std::lock_guard<std::mutex> guard(m_JobMutex);
        return m_Jobs.empty();
    }

private:
    std::deque<std::unique_ptr<Job>> m_Jobs;
    std::vector<std::unique_ptr<Worker>> m_Workers;
    std::mutex m_JobMutex;
    std::condition_variable m_JobCV;
};

class JobSystem
{
private:
    JobSystem();

public:
    ~JobSystem();
    uint32_t RequestWorker();
    bool ReleaseWorker(uint32_t workerId);

    void SortJobs(ScheduleStrategy strategy);
    bool GetFirstJob(std::unique_ptr<Job> &job);
    bool GetFirstJob(std::unique_ptr<Job> &job, uint32_t workerID);

    bool SubmitJob(std::unique_ptr<Job> &job);
    bool Run();
    bool IsFinished() const;

public:
    static JobSystem *CreateJobSystem();
    static void DestroyJobSystem(JobSystem *jobSystem);

private:
    uint32_t m_MaxNumOfWorkers = std::thread::hardware_concurrency();
    bool m_bIsRunning = false;
    std::vector<std::unique_ptr<Worker>> m_Workers;
    std::deque<std::unique_ptr<Job>> m_Jobs;
    std::condition_variable m_JobCV;
    std::mutex m_JobMutex;
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
    jobSystem->Run();
}
#endif