#pragma once
#include "Common/pch.h"
#include "Engine/EngineInterface.h"
#include "Engine/EngineTimer.h"
class RenderModule;
class PhysicsModule;
class Engine
{
private:
    Engine();
    ~Engine();

public:
    static Engine *CreateEngine();
    static void DestroyEngine(Engine *engine);
    void Init();
    void Start();
    void Update();
    void Stop();
    void Shutdown();

private:
    bool m_bIsRunning;
    EngineTimer m_Timer;
    PhysicsModule *m_physicsModule = nullptr;
    RenderModule *m_renderModule = nullptr;

    SharedPtr<IRenderSystem> m_RenderSystem = nullptr;
    std::vector<SharedPtr<IPlugin>> m_Plugins;
};

inline bool GetEngine(Engine **engine);
