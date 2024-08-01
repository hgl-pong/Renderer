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

    friend inline bool operator<(const UniquePtr<Job> &a, const UniquePtr<Job> &b);

    const uint8_t &GetPriority() const
    {
        return m_Priority;
    }

private:
    bool m_IsFinished = false;
    uint8_t m_Priority = 0;
    std::function<void()> m_ExecuteFunction;
};

inline bool operator<(const UniquePtr<Job> &a, const UniquePtr<Job> &b)
{
    return a->m_Priority < b->m_Priority;
};

class Worker
{
public:
    Worker() = delete;
    Worker(JobPipeLine *pipeLine);
    ~Worker();
    void Run();
    void Reset();
    bool IsBusy() const;
    bool IsInPipeLine() const;
    bool SetPipeLine(JobPipeLine *pipeLine);

private:
    void Stop();
    bool WakeUp();

private:
    std::atomic<bool> m_IsRunning;
    std::thread m_Thread;
    UniquePtr<Job> m_CurrentJob;
    JobPipeLine *m_PipeLine = nullptr;
    std::mutex *m_Mutex = nullptr;
    std::condition_variable *m_Condition = nullptr;
};

class JobPipeLine
{
public:
    JobPipeLine() = delete;
    JobPipeLine(uint32_t workerCount);
    ~JobPipeLine() = default;
    JobPipeLine(const JobPipeLine &) = delete;
    JobPipeLine(JobPipeLine &&) = delete;
    JobPipeLine &operator=(const JobPipeLine &) = delete;

    void PushJob(UniquePtr<Job> &job);
    bool PopJob(UniquePtr<Job> &job);
    bool TransferInWorker(UniquePtr<Worker> &worker);
    bool TransferInWorkers(std::vector<UniquePtr<Worker>> &workers);
    bool TransferOutWorker(UniquePtr<Worker> &worker);
    bool TransferOutAllWorkers(std::vector<UniquePtr<Worker>> &workers);
    bool IsIdle();
    void SortJobs(ScheduleStrategy strategy);
    bool BorrowWorker(JobPipeLine *pipeLine);
    bool CallReturnWorker(JobPipeLine *pipeLine);

private:
    friend class Worker;
    std::deque<UniquePtr<Job>> m_Jobs;
    std::vector<UniquePtr<Worker>> m_Workers;
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
    bool SubmitJob(UniquePtr<Job> &job, const uint32_t pipeLineID = 0);

public:
    static JobSystem *CreateJobSystem();
    static void DestroyJobSystem(JobSystem *jobSystem);

private:
    uint32_t m_MaxNumOfWorkers = std::thread::hardware_concurrency();
    uint32_t m_CurrentNumOfWorkers = 0;
    std::vector<UniquePtr<JobPipeLine>> m_JobPipeLines;
    std::mutex m_Mutex;
};

inline bool GetJobSystem(JobSystem **jobSystem);

#ifdef MODULE_TEST
inline void TestJobSystem()
{
    UniquePtr<JobSystem> jobSystem(JobSystem::CreateJobSystem());
    for (int i = 0; i < 10000; i++)
    {
        std::string log = "Job " + std::to_string(i) + " executed\n";
        UniquePtr<Job> job = std::make_unique<Job>([log]()
                                                   { HLOG_INFO(log.c_str()); });
        jobSystem->SubmitJob(job);
    }
    jobSystem->SortJobs(ScheduleStrategy::PRIORITY);
}
#endif