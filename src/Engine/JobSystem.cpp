#include "Common/pch.h"
#include "Engine/JobSystem.h"

static JobSystem *jobSystemSingleton = nullptr;
static std::condition_variable jobSystemCV;

class Factory
{
public:
    Factory() = default;
    ~Factory() = default;

private:
};

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
    if (pipeLineID >= m_JobPipeLines.size() && pipeLineID != UINT_MAX)
    {
        HLOG_ERROR("Invalid pipeline id\n");
        return false;
    }
    if (pipeLineID == UINT_MAX)
    {
        m_JobPipeLines[0]->PushJob(job);
        HLOG_ERROR("Failed to submit job\n");
        return false;
    }
    m_JobPipeLines[pipeLineID]->PushJob(job);
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