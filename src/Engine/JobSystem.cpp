#include "Common/pch.h"
#include "Engine/JobSystem.h"

class Worker
{
public:
    Worker(uint32_t workerID = UINT_MAX) : m_IsRunning(true), m_WorkerID(workerID)
    {
        m_Thread = std::thread(&Worker::Run, this);
    }

    ~Worker()
    {
        Stop();
        if (m_Thread.joinable())
        {
            m_Thread.join();
        }
    }

    void Run()
    {
        std::unique_ptr<Job> job;
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
                job->Execute();
                job = nullptr;
            }
        }
    }

    void Execute(std::unique_ptr<Job> &job)
    {
        std::lock_guard<std::mutex> guard(m_Mutex);
        if (m_CurrentJob)
        {
            HLOG_ERROR("Worker is busy, can't execute job");
            return;
        }
        m_CurrentJob = std::move(job);
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
    std::unique_ptr<Job> m_CurrentJob;
};

JobSystem::JobSystem()
{
    for (int i = 0; i < m_MaxNumOfWorkers / 2; i++)
        m_Workers.push_back(std::make_unique<Worker>());
    HLOG_INFO("JobSystem initialized, number of workers: %d\n", m_MaxNumOfWorkers);
}

JobSystem::~JobSystem()
{
}

uint32_t JobSystem::RequestWorker()
{
    uint32_t id = static_cast<uint32_t>(m_Workers.size());
    if (id >= m_MaxNumOfWorkers)
    {
        HLOG_ERROR("No available worker");
        return UINT_MAX;
    }
    m_Workers.push_back(std::make_unique<Worker>(id));
    HLOG_INFO("Worker requested");
    return id;
}

bool JobSystem::ReleaseWorker(uint32_t workerId)
{
    for (auto it = m_Workers.begin(); it != m_Workers.end(); it++)
    {
        if ((*it)->GetWorkerID() == workerId)
        {
            m_Workers.erase(it);
            HLOG_INFO("Worker released");
            return true;
        }
    }
    HLOG_ERROR("Not Found worker");
    return false;
}

bool JobSystem::SubmitJob(std::unique_ptr<Job> &job)
{
    std::lock_guard<std::mutex> guard(m_JobMutex);
    m_Jobs.push_back(std::move(job));
    m_JobCV.notify_one();
    return true;
}

void JobSystem::AllocateJob()
{
    std::lock_guard<std::mutex> guard(m_JobMutex);
    for (auto &worker : m_Workers)
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
    for (auto &worker : m_Workers)
    {
        if (worker->IsBusy())
            return false;
    }
    return true;
}