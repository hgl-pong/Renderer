#include "Application.h"
#include "RenderModule.h"
#include "PhysicsModule.h"

Application::Application()
{
}

Application::~Application()
{
    delete m_physicsModule;
    delete m_renderModule;
}

void Application::Init()
{
    m_physicsModule = new PhysicsModule();
    m_physicsModule->Init();

    m_renderModule = new RenderModule();
    m_renderModule->Init();
}

void Application::Run()
{
    // while (!glfwWindowShouldClose(m_renderModule->GetWindow()))
    while (true)
    {
        m_physicsModule->Update(1.0f / 60.0f);
        m_renderModule->Update(1.0f / 60.0f);
    }
}

void Application::Shutdown()
{
    m_renderModule->Shutdown();
    m_physicsModule->Shutdown();
}
