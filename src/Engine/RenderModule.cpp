#include "Engine/RenderModule.h"

RenderModule::RenderModule(std::string name)
    : m_windowName(name)
{
    name = "RenderModule";
    m_window = nullptr;
}

RenderModule::~RenderModule()
{
}

void RenderModule::Init()
{
}

void RenderModule::Update(float dt)
{
}

void RenderModule::Shutdown()
{
}
