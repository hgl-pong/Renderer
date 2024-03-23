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

class Worker;
class JobSystem
{
private:
    JobSystem();

public:
    ~JobSystem();
    uint32_t RequestWorker();
    bool ReleaseWorker(uint32_t workerId);

    void SortJobs(ScheduleStrategy strategy)
    {
        switch (strategy)
        {
        case ScheduleStrategy::FIFO:
            break;
        case ScheduleStrategy::LIFO:
            break;
        case ScheduleStrategy::PRIORITY:
            std::sort(m_Jobs.begin(), m_Jobs.end(), [](const std::unique_ptr<Job> &a, const std::unique_ptr<Job> &b)
                      { return a < b; });
            break;
        }
    }
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