#include "Common/pch.h"
#include "Engine/EngineTimer.h"
EngineTimer::EngineTimer()
{
    m_DeltaTime = -1.0;
    m_BaseTime = std::chrono::steady_clock::now();
    m_PausedTime = std::chrono::steady_clock::now();
    m_StopTime = std::chrono::steady_clock::now();
    m_PrevTime = std::chrono::steady_clock::now();
    m_CurrTime = std::chrono::steady_clock::now();
    m_Stopped = false;
}

EngineTimer::~EngineTimer()
{
}

void EngineTimer::Start()
{
    auto startTime = std::chrono::steady_clock::now();
    if (m_Stopped)
    {
        m_PausedTime += startTime - m_StopTime;
        m_PrevTime = startTime;
        m_StopTime = std::chrono::steady_clock::now();
        m_Stopped = false;
    }
}

void EngineTimer::Stop()
{
    if (!m_Stopped)
    {
        m_StopTime = std::chrono::steady_clock::now();
        m_Stopped = true;
    }
}

void EngineTimer::Reset()
{
    auto time = std::chrono::duration_cast<std::chrono::duration<double>>(m_StopTime - m_BaseTime);
    m_BaseTime = std::chrono::steady_clock::now();
    m_PrevTime = std::chrono::steady_clock::now();
    m_StopTime = std::chrono::steady_clock::now();
    m_Stopped = false;
}

void EngineTimer::Tick()
{
    if (m_Stopped)
    {
        m_DeltaTime = 0.0;
        return;
    }
    m_CurrTime = std::chrono::steady_clock::now();
    m_DeltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(m_CurrTime - m_PrevTime).count();
    m_PrevTime = m_CurrTime;
    if (m_DeltaTime < 0.0)
    {
        m_DeltaTime = 0.0;
    }
}

float EngineTimer::GetDeltaTime() const
{
    return static_cast<float>(m_DeltaTime);
}

float EngineTimer::GetTotalTime() const
{
    if (m_Stopped)
    {
        return static_cast<float>(std::chrono::duration_cast<std::chrono::duration<double>>(m_StopTime - m_BaseTime).count());
    }
    else
    {
        return static_cast<float>(std::chrono::duration_cast<std::chrono::duration<double>>(m_CurrTime - m_BaseTime).count());
    }
}