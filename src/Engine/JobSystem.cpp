#include "Common/pch.h"
#include "Engine/JobSystem.h"

class Worker
{
public:
    Worker() : m_IsRunning(true), m_WorkerID(UINT_MAX)
    {
        m_Thread = std::thread(&Worker::Run, this);
    }

    ~Worker()
    {
        Stop();
        m_Thread.join();
    }

    void Run()
    {
        std::function<void()> job;
        while (m_IsRunning)
        {
            {
                std::unique_lock<std::mutex> lock(m_Mutex);
                m_Condition.wait(lock, [this]
                                 { return !m_IsRunning || m_CurrentJob != nullptr; });
                if (!m_IsRunning)
                    break;
                job = std::move(m_CurrentJob);
                m_CurrentJob = nullptr;
            }
            if (job)
            {
                job();
                job = nullptr;
            }
        }
    }

    void Execute(Job *job)
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        if (m_CurrentJob)
        {
            HLOG_ERROR("Worker is busy, can't execute job");
            return;
        }
        m_CurrentJob = std::bind(&Job::Execute, job);
        WakeUp();
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
        WakeUp();
    }

    bool WakeUp()
    {
        m_Condition.notify_one();
        return true;
    }

    uint32_t m_WorkerID;
    mutable std::mutex m_Mutex;
    std::condition_variable m_Condition;
    std::atomic<bool> m_IsRunning;
    std::thread m_Thread;
    std::function<void()> m_CurrentJob = nullptr;
};

JobSystem::JobSystem()
{
    for (int i = 0; i < std::thread::hardware_concurrency(); i++)
        m_Workers.push_back(new Worker());
}

JobSystem::~JobSystem()
{
    for (auto worker : m_Workers)
    {
        delete worker;
    }
}

uint32_t JobSystem::RequestWorker()
{
    uint32_t i = 0;
    for (auto worker : m_Workers)
    {
        if (!worker->IsDedicated())
        {
            worker->SetWorkerID(i);
            HLOG_INFO("Worker assigned");
            return i;
        }
        i++;
    }
    HLOG_ERROR("No available worker");
    return UINT_MAX;
}

bool JobSystem::ReleaseWorker(uint32_t workerId)
{
    auto &worker = m_Workers[workerId];
    if (worker->IsDedicated() && worker->GetWorkerID() == workerId)
    {
        worker->SetWorkerID(UINT_MAX);
        HLOG_INFO("Worker released");
        return true;
    }
    HLOG_ERROR("Not Found worker");
    return false;
}

bool JobSystem::SubmitJob(Job *job)
{
    m_Jobs.push_back(job);
    return true;
}

void JobSystem::AllocateJob()
{
    for (auto worker : m_Workers)
    {
        if (!worker->IsBusy())
        {
            if (!m_Jobs.empty())
            {
                worker->Execute(m_Jobs.front());
                m_Jobs.pop_front();
            }
        }
    }
}

bool JobSystem::Run()
{
    while (!IsFinished())
    {
        AllocateJob();
    }
    return true;
}

bool JobSystem::IsFinished() const
{
    for (auto worker : m_Workers)
    {
        if (worker->IsBusy())
            return false;
    }
    return true;
}