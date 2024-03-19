#include "Engine/Engine.h"
#include "Engine/RenderModule.h"
#include "Engine/PhysicsModule.h"

static Engine *engineSingleton = nullptr;

Engine::Engine()
{
}

Engine::~Engine()
{
    delete m_physicsModule;
    delete m_renderModule;
}

void Engine::Init()
{
    m_physicsModule = new PhysicsModule();
    m_physicsModule->Init();

    m_renderModule = new RenderModule("Renderer");
    m_renderModule->Init();
}

void Engine::Start()
{
    m_bIsRunning = true;
    m_Timer.Start();
}

void Engine::Update()
{
    m_Timer.Tick();
    float deltaTime = m_Timer.GetDeltaTime();
    m_physicsModule->Update(deltaTime);
    m_renderModule->Update(deltaTime);
}

void Engine::Stop()
{
    m_bIsRunning = false;
    m_Timer.Stop();
}

void Engine::Shutdown()
{
    m_renderModule->Shutdown();
    m_physicsModule->Shutdown();
}

Engine *Engine::CreateEngine()
{
    if (engineSingleton == nullptr)
    {
        engineSingleton = new Engine();
    }
    return engineSingleton;
}

void Engine::DestroyEngine(Engine *engine)
{
    if (engineSingleton != nullptr)
    {
        delete engineSingleton;
        engineSingleton = nullptr;
    }
}

bool GetEngine(Engine **engine)
{
    if (engineSingleton != nullptr)
    {
        *engine = engineSingleton;
        return true;
    }
    HLOG_ERROR("Engine is not initialized");
    return false;
}