#pragma once
#include <Module.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class RenderModule : public Module
{
public:
    RenderModule(std::string name);
    virtual ~RenderModule();

    virtual void Init();
    virtual void Update(float dt);
    virtual void Shutdown();

private:
    std::string m_windowName;
    GLFWwindow *m_window = nullptr;
};
