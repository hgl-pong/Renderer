#include "RenderModule.h"

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
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    m_window = glfwCreateWindow(1024, 768, m_windowName.c_str(), NULL, NULL);
    if (m_window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window); // Initialize GLEW
    glewExperimental = true;          // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW.\n");
        return;
    }
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
}

void RenderModule::Update(float dt)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void RenderModule::Shutdown()
{
    glfwTerminate();
}
