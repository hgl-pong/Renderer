#include "Common/pch.h"
#include "Engine/JobSystem.h"

static JobSystem *jobSystemSingleton = nullptr;
static std::condition_variable jobSystemCV;
Worker::Worker(JobPipeLine *pipeLine) : m_IsRunning(true)
{
    SetPipeLine(pipeLine);
    m_Thread = std::thread(&Worker::Run, this);
}

Worker::~Worker()
{
    if (m_Mutex != nullptr && m_Condition != nullptr)
    {
        std::unique_lock<std::mutex> lock(*m_Mutex);
        m_Condition->wait(lock, [&]
                          { return !m_IsRunning && m_PipeLine->IsIdle(); });
    }
    if (m_Thread.joinable())
    {
        m_Thread.join();
    }
}

void Worker::Run()
{
    while (m_IsRunning)
    {
        if (m_Mutex == nullptr || m_Condition == nullptr)
        {
            HLOG_ERROR("Mutex or Condition is not set\n");
            break;
        }
        {
            std::unique_lock<std::mutex> lock(*m_Mutex);
            m_Condition->wait(lock, [this]
                              { return !m_IsRunning || (!m_PipeLine->IsIdle()); });
            if (m_PipeLine)
            {
                m_PipeLine->PopJob(m_CurrentJob);
            }
            if (!m_IsRunning)
                break;
            m_CurrentJob->Execute();
            m_CurrentJob = nullptr;
        }
    }
}

void Worker::Reset()
{
    if (m_Mutex != nullptr && m_Condition != nullptr)
    {
        std::unique_lock<std::mutex> lock(*m_Mutex);
        m_Condition->wait(lock, [&]
                          { return !m_IsRunning && m_PipeLine->IsIdle(); });
    }
    if (m_Thread.joinable())
    {
        m_Thread.join();
    }
    m_Thread = std::thread(&Worker::Run, this);
}

bool Worker::IsBusy() const
{
    if (m_Mutex == nullptr)
        return false;
    std::lock_guard<std::mutex> guard(*m_Mutex);
    return m_CurrentJob != nullptr;
}

bool Worker::IsInPipeLine() const
{
    return m_Mutex != nullptr && m_Condition != nullptr;
}

bool Worker::SetPipeLine(JobPipeLine *pipeLine)
{
    if (IsBusy())
    {
        HLOG_ERROR("Worker is busy\n");
        return false;
    }
    if (pipeLine == nullptr)
    {
        HLOG_ERROR("PipeLine is nullptr\n");
        return false;
    }
    m_PipeLine = pipeLine;
    m_Mutex = &pipeLine->m_JobMutex;
    m_Condition = &pipeLine->m_JobCV;
    return true;
}

void Worker::Stop()
{
    {
        std::lock_guard<std::mutex> guard(*m_Mutex);
        m_IsRunning = false;
    }
    m_Condition->notify_all();
}

bool Worker::WakeUp()
{
    {
        std::lock_guard<std::mutex> guard(*m_Mutex);
        m_IsRunning = true;
    }
    m_Condition->notify_all();
    return true;
}

JobSystem::JobSystem()
{
    HLOG_INFO("JobSystem created\n");
    m_JobPipeLines.push_back(std::make_unique<JobPipeLine>(m_MaxNumOfWorkers / 2));
    m_CurrentNumOfWorkers = m_MaxNumOfWorkers / 2;
}

JobSystem::~JobSystem()
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    for (auto &pipeline : m_JobPipeLines)
    {
        pipeline.reset();
    }
    m_JobPipeLines.clear();
}

uint32_t JobSystem::CreateNewPipeLine()
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    if (m_CurrentNumOfWorkers >= m_MaxNumOfWorkers)
    {
        HLOG_ERROR("No more workers can be created\n");
        return UINT_MAX;
    }
    uint32_t id = m_JobPipeLines.size();
    m_JobPipeLines.push_back(std::make_unique<JobPipeLine>(1));
    HLOG_INFO("New pipeline created with id %d\n", id);
    m_CurrentNumOfWorkers++;
    return id;
}

bool JobSystem::DestroyPipeLine(uint32_t pipeLineID)
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    if (pipeLineID >= m_JobPipeLines.size())
    {
        HLOG_ERROR("Invalid pipeline id\n");
        return false;
    }
    m_JobPipeLines[pipeLineID].reset();
    m_JobPipeLines.erase(m_JobPipeLines.begin() + pipeLineID);
    m_CurrentNumOfWorkers--;
    return true;
}

bool JobSystem::SubmitJob(std::unique_ptr<Job> &job, const uint32_t pipeLineID)
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    if (pipeLineID >= m_JobPipeLines.size())
    {
        HLOG_ERROR("Invalid pipeline id\n");
        return false;
    }
    m_JobPipeLines[pipeLineID]->PushJob(job);
    HLOG_INFO("Job submitted\n");
    return true;
}

void JobSystem::SortJobs(ScheduleStrategy strategy)
{
    std::lock_guard<std::mutex> guard(m_Mutex);
    for (auto &pipeline : m_JobPipeLines)
    {
        pipeline->SortJobs(strategy);
    }
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