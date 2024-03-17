#pragma once
#include "Common/Types.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
class PhysicsModule;
class RenderModule;
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
