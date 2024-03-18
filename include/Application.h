#pragma once
#include "Common/pch.h"
class RenderModule;
class PhysicsModule;
class Application
{
public:
    Application();
    ~Application();

    void Init();
    void Run();
    void Shutdown();

private:
    PhysicsModule *m_physicsModule = nullptr;
    RenderModule *m_renderModule = nullptr;
};
