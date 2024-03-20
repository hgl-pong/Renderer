#include "Common/pch.h"
#include "Engine/JobSystem.h"
class Worker
{
public:
    Worker()
    {
        m_Thread = std::thread(&Worker::Run, this);
    }

    ~Worker()
    {
        m_Thread.join();
    }

    void Run()
    {
        while (true)
        {
            if (!m_Jobs.empty())
            {
                m_Jobs.front()->Execute();
                m_Jobs.pop_front();
            }
        }
    }

    uint32_t GetJobCount()
    {
        return m_Jobs.size();
    }

    void AddJob(Job *job)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Jobs.push_back(job);
    }

    bool IsBusy()
    {
        return !m_Jobs.empty();
    }

    bool IsDedicated()
    {
        return m_UserData != nullptr;
    }

    void SetUserData(void *userData)
    {
        m_UserData = userData;
    }

    void *GetUserData() const
    {
        return m_UserData;
    }

private:
    void *m_UserData = nullptr;
    std::mutex m_Mutex;
    std::condition_variable m_ConditionVariable;
    std::thread m_Thread;
    std::deque<Job *> m_Jobs;
};

JobSystem::JobSystem()
{
    for (int i = 0; i < std::thread::hardware_concurrency(); i++)
        m_Workers.push_back(new Worker());
}

bool JobSystem::RequestWorker(void *userData)
{
    for (auto worker : m_Workers)
    {
        if (!worker->IsDedicated())
        {
            worker->SetUserData(userData);
            HLOG_INFO("Worker assigned");
            return true;
        }
    }
    HLOG_ERROR("No available worker");
    return false;
}

bool JobSystem::ReleaseWorker(void *userData)
{
    for (auto worker : m_Workers)
    {
        if (worker->IsDedicated() && worker->GetUserData() == userData)
        {
            worker->SetUserData(nullptr);
            HLOG_INFO("Worker released");
            return true;
        }
    }
    HLOG_ERROR("Not Found worker");
    return false;
}