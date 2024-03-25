#include "Common/pch.h"
#include "Engine/JobSystem.h"

static JobSystem *jobSystemSingleton = nullptr;
static std::condition_variable jobSystemCV;
std::atomic<int> counter(0);

class Factory
{
public:
    Factory() = default;
    ~Factory() = default;

private:
};

JobSystem::JobSystem()
{
    for (int i = 0; i < m_MaxNumOfWorkers / 2; i++)
        m_Workers.push_back(std::make_unique<Worker>());
    HLOG_INFO("Device Supported threads: %d\n", m_MaxNumOfWorkers);
    HLOG_INFO("JobSystem initialized, number of workers: %d\n", m_Workers.size());
}

JobSystem::~JobSystem()
{
    std::unique_lock<std::mutex> lock(m_JobMutex);
    jobSystemCV.wait(lock, [&]
                     { return m_Jobs.empty() && IsFinished(); });
    for (auto &worker : m_Workers)
    {
        worker.reset();
    }
    m_Workers.clear();
    jobSystemSingleton = nullptr;
    HLOG_INFO("JobSystem destroyed\n");
}

uint32_t JobSystem::RequestWorker()
{
    uint32_t id = static_cast<uint32_t>(m_Workers.size());
    if (id >= m_MaxNumOfWorkers)
    {
        HLOG_ERROR("No available worker\n");
        return UINT_MAX;
    }
    m_Workers.push_back(std::make_unique<Worker>(id));
    HLOG_INFO("Worker requested\n");
    return id;
}

bool JobSystem::ReleaseWorker(uint32_t workerId)
{
    for (auto it = m_Workers.begin(); it != m_Workers.end(); it++)
    {
        if ((*it)->GetWorkerID() == workerId)
        {
            m_Workers.erase(it);
            HLOG_INFO("Worker released\n");
            return true;
        }
    }
    HLOG_ERROR("Not Found worker\n");
    return false;
}

bool JobSystem::SubmitJob(std::unique_ptr<Job> &job)
{
    std::lock_guard<std::mutex> guard(m_JobMutex);
    if (job == nullptr)
    {
        HLOG_ERROR("Submitting a null job\n");
        return false;
    }
    m_Jobs.push_back(std::move(job));
    m_JobCV.notify_all();
    return true;
}

bool JobSystem::Run()
{
    for (auto &worker : m_Workers)
    {
        worker->Activate();
    }
    return true;
}

bool JobSystem::IsFinished() const
{
    for (auto &worker : m_Workers)
    {
        if (!worker->IsBusy())
        {
            return false;
        }
    }
    return true;
}

void JobSystem::SortJobs(ScheduleStrategy strategy)
{
    std::lock_guard<std::mutex> guard(m_JobMutex);
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

bool JobSystem::GetFirstJob(std::unique_ptr<Job> &job)
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

bool JobSystem::GetFirstJob(std::unique_ptr<Job> &job, uint32_t workerID)
{
    std::lock_guard<std::mutex> guard(m_JobMutex);
    if (m_Jobs.empty())
    {
        m_JobCV.notify_all();
        return false;
    }
    std::deque<std::unique_ptr<Job>> tempQueue;
    bool result = false;
    while (!m_Jobs.empty())
    {
        if (m_Jobs.front()->GetWorkerID() == workerID)
        {
            job = std::move(m_Jobs.front());
            m_Jobs.pop_front();
            result = true;
            break;
        }
        tempQueue.push_back(std::move(m_Jobs.front()));
        m_Jobs.pop_front();
    }

    while (!m_Jobs.empty())
    {
        tempQueue.push_back(std::move(m_Jobs.front()));
        m_Jobs.pop_front();
    }
    m_Jobs = std::move(tempQueue);
    return result;
}

JobSystem *JobSystem::CreateJobSystem()
{
    if (jobSystemSingleton == nullptr)
    {
        jobSystemSingleton = new JobSystem();
    }
    else
    {
        HLOG_ERROR("JobSystem is already initialized,JobSystem class is a singleton\n");
    }
    return jobSystemSingleton;
}

void JobSystem::DestroyJobSystem(JobSystem *jobSystem)
{
    if (jobSystemSingleton != nullptr)
    {
        delete jobSystemSingleton;
        jobSystemSingleton = nullptr;
    }
}

bool GetJobSystem(JobSystem **jobSystem)
{
    if (jobSystemSingleton != nullptr)
    {
        *jobSystem = jobSystemSingleton;
        return true;
    }
    HLOG_ERROR("JobSystem is not initialized\n");
    return false;
}