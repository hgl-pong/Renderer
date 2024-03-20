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
    Job(std::function<void()> executeFunction)
        : m_ExecuteFunction(executeFunction), m_IsFinished(false)
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

    bool operator<(const Job &other) const
    {
        return m_Priority < other.m_Priority;
    }

private:
    bool m_IsFinished = false;
    uint8_t m_Priority = 0;
    std::function<void()> m_ExecuteFunction;
};
class Worker;
class JobSystem
{
public:
    JobSystem();
    bool RequestWorker(void *userData);
    bool ReleaseWorker(void *userData);

    template <class F, class... Args>
    auto Enqueue(F &&f, Args &&...args)
        -> std::future<typename std::invoke_result<F(Args...)>::type>
    {
        using return_type = typename std::invoke_result<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            m_Jobs.push_back(new Job((*task)()));
        }
        return res;
    }

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

private:
    bool m_bIsRunning = false;
    std::vector<Worker *> m_Workers;
    std::deque<Job *> m_Jobs;
};