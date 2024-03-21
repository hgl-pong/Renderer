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
        m_ExecuteFunction();
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

    bool operator<(const Job &other) const
    {
        return m_Priority < other.m_Priority;
    }

private:
    uint32_t m_WorkerID = UINT_MAX;
    bool m_IsFinished = false;
    uint8_t m_Priority = 0;
    std::function<void()> m_ExecuteFunction;
};
class Worker;
class JobSystem
{
public:
    JobSystem();
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
            std::sort(m_Jobs.begin(), m_Jobs.end(), [](Job *a, Job *b)
                      { return *a < *b; });
            break;
        }
    }

    void AllocateJob();
    bool SubmitJob(Job *job);
    bool Run();
    bool IsFinished() const;

private:
    bool m_bIsRunning = false;
    std::vector<Worker *> m_Workers;
    std::deque<Job *> m_Jobs;
};
#ifdef MODULE_TEST
inline void TestJobSystem()
{
    JobSystem jobSystem;
    for (int i = 0; i < 10000; i++)
    {
        std::string log = "Job " + std::to_string(i)+" executed\n";
        Job *job = new Job([log]()
                           { HLOG_INFO(log.c_str()); });
        jobSystem.SubmitJob(job);
    }
    jobSystem.SortJobs(ScheduleStrategy::PRIORITY);
    jobSystem.AllocateJob();
    jobSystem.Run();
}
#endif