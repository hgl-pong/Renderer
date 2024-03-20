#pragma once
#include "Common/pch.h"

class Worker;
class JobSystem
{
private:
    JobSystem();
    bool RequestWorker(void* userData);
    bool ReleaseWorker(void* userData);

public:
    bool m_bIsRunning = false;
    std::vector<Worker*> m_Workers;
};