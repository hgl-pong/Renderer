#pragma once
#include "pch.h"

class EngineTimer
{
public:
    EngineTimer();
    ~EngineTimer();

    void Start();
    void Stop();
    void Reset();
    void Tick();

    float GetDeltaTime() const;
    float GetTotalTime() const;

private:
    double m_DeltaTime;

    std::chrono::steady_clock::time_point m_BaseTime;
    std::chrono::steady_clock::time_point m_PausedTime;
    std::chrono::steady_clock::time_point m_StopTime;
    std::chrono::steady_clock::time_point m_PrevTime;
    std::chrono::steady_clock::time_point m_CurrTime;

    bool m_Stopped;
};