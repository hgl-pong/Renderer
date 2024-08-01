#include "Common/pch.h"
#include "Engine/Window.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "../RenderSystem/imgui/imgui_impl_glfw.h"
IWindow *CreateRenderWindow()
{
#ifdef EDITOR_MODE
    return new EditorWindow();
#else
    return nullptr;
#endif
}

enum class ImguiWindowType
{
    GameView,
};

class ImguiWindow
{
public:
    virtual void Render() = 0;
    virtual ImguiWindowType GetType() = 0;
    virtual void SetOpen(bool open) { m_IsOpen = open; }

protected:
    std::string m_Title;
    bool m_IsOpen = true;
};

class ImguiGameViewWindow : virtual public ImguiWindow
{
public:
    ImguiGameViewWindow()
    {
        m_Title = "GameView";
    }
    void Render() override
    {
        if (!m_IsOpen)
            return;
        ImGui::Begin(m_Title.c_str(), &m_IsOpen);
        ImVec2 size = ImGui::GetContentRegionAvail();
        // ImGui::Image((void *)m_TextureID, size, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
    }
    ImguiWindowType GetType() override
    {
        return ImguiWindowType::GameView;
    }
};

class ImguiWindowsFactory
{
public:
    ImguiWindowsFactory(SharedPtr<GLFWwindow> &window)
    {
        m_Window = window;
    }
    void SetUp(const RenderSystemType &type)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        switch (type)
        {
        case RenderSystemType::Vulkan:
            ImGui_ImplGlfw_InitForVulkan(m_Window.get(), true);
            break;
        case RenderSystemType::OpenGL:
            ImGui_ImplGlfw_InitForOpenGL(m_Window.get(), true);
            break;
        case RenderSystemType::DirectX11:
            break;
        case RenderSystemType::DirectX12:
            break;
        default:
            break;
        }
    }

    void ShutDown()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ReSet(const RenderSystemType &type)
    {
        ShutDown();
        SetUp(type);
    }

    void ProcessEvents()
    {
    }

    void RenderWindows()
    {
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        for (auto &window : m_Windows)
        {
            window->Render();
        }

        ImGui::Render();
    }

    ImguiWindow *CreateGameViewWindow()
    {
        for (auto &window : m_Windows)
        {
            if (window->GetType() == ImguiWindowType::GameView)
                return window.get();
        }
        m_Windows.push_back(SharedPtr<ImguiWindow>(new ImguiGameViewWindow()));
        return m_Windows.back().get();
    }

    void DestroyGameViewWindow()
    {
        for (auto iter = m_Windows.begin(); iter != m_Windows.end(); ++iter)
        {
            if ((*iter)->GetType() == ImguiWindowType::GameView)
            {
                m_Windows.erase(iter);
                return;
            }
        }
    }

private:
    SharedPtr<GLFWwindow> m_Window;
    std::vector<SharedPtr<ImguiWindow>> m_Windows;
};

EditorWindow::EditorWindow()
    : m_RenderSystem(nullptr),
      m_Window(nullptr),
      m_Title(""),
      m_Width(800),
      m_Height(600),
      m_VSync(true),
      m_FullsScreen(false),
      m_Visible(true),
      m_MouseCursorVisible(true),
      m_ClearColor(MathLib::HVector4(0.0f, 0.0f, 0.0f, 1.0f)),
      m_Position(MathLib::HVector2(0.0f, 0.0f))
{
}

EditorWindow::~EditorWindow()
{
    DestroyRenderWindow();
}

void EditorWindow::CreateRenderWindow(const std::string &title, int width, int height)
{
    m_Title = title;
    m_Width = width;
    m_Height = height;
    try
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = SharedPtr<GLFWwindow>(glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr));
        HASSERT_LOG(m_Window != nullptr, "glfwCreateWindow failed");
        m_ImguiWindowsFactory = SharedPtr<ImguiWindowsFactory>(new ImguiWindowsFactory(m_Window));
        HASSERT_LOG(m_ImguiWindowsFactory != nullptr, "Create ImguiWindowsFactory failed");
        m_ImguiWindowsFactory->SetUp(GetRenderSystemType());
        m_ImguiWindowsFactory->CreateGameViewWindow();
    }
    catch (const std::exception &e)
    {
        HLOG_ERROR(e.what());
    }
}

void EditorWindow::DestroyRenderWindow()
{
    if (m_Window != nullptr)
    {
        glfwDestroyWindow(m_Window.get());
        m_Window = nullptr;
    }
    if (m_ImguiWindowsFactory != nullptr)
    {
        m_ImguiWindowsFactory->ShutDown();
        m_ImguiWindowsFactory = nullptr;
    }
    glfwTerminate();
}

void EditorWindow::SetTitle(const std::string &title)
{
    if (m_Window == nullptr)
        return;
    m_Title = title;
    glfwSetWindowTitle(m_Window.get(), m_Title.c_str());
}

void EditorWindow::SetSize(int width, int height)
{
    if (m_Window == nullptr)
        return;
    m_Width = width;
    m_Height = height;
    glfwSetWindowSize(m_Window.get(), m_Width, m_Height);
}

void EditorWindow::SetVSync(bool vsync)
{
    if (m_Window == nullptr)
        return;
    m_VSync = vsync;
}

void EditorWindow::SetFullscreen(bool fullscreen)
{
    if (m_Window == nullptr)
        return;
    m_FullsScreen = fullscreen;
    GLFWwindow *window = m_Window.get();
    int windowed_xpos, windowed_ypos, windowed_width, windowed_height;
    if (glfwGetWindowMonitor(window))
    {
        // 当前是全屏，切换到窗口模式
        glfwSetWindowMonitor(window, NULL, windowed_xpos, windowed_ypos, windowed_width, windowed_height, 0);
    }
    else
    {
        // 当前是窗口模式，保存窗口位置和大小
        glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
        glfwGetWindowSize(window, &windowed_width, &windowed_height);

        // 切换到全屏模式
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
}

void EditorWindow::SetVisible(bool visible)
{
    if (m_Window == nullptr)
        return;
    m_Visible = visible;
    if (m_Visible)
        glfwShowWindow(m_Window.get());
    else
        glfwHideWindow(m_Window.get());
}

void EditorWindow::SetMouseCursorVisible(bool visible)
{
    if (m_Window == nullptr)
        return;
    m_MouseCursorVisible = visible;
    glfwSetInputMode(m_Window.get(), GLFW_CURSOR, m_MouseCursorVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void EditorWindow::SetPosition(int x, int y)
{
    if (m_Window == nullptr)
        return;
    m_Position = MathLib::HVector2(x, y);
    glfwSetWindowPos(m_Window.get(), x, y);
}

void EditorWindow::SetIcon(const std::string &iconPath)
{
    if (m_Window == nullptr)
        return;
}

void EditorWindow::SetClearColor(const MathLib::HVector4 &color)
{
    m_ClearColor = color;
}

void EditorWindow::Clear()
{
    if (m_RenderSystem)
        m_RenderSystem->Clear(m_ClearColor);

    if (m_Window == nullptr)
        return;
}

void EditorWindow::Display()
{
    if (m_Window == nullptr)
        return;
    if (m_RenderSystem)
    {
        m_RenderSystem->BeginFrame();
        m_RenderSystem->EndFrame();
    }
    if (m_ImguiWindowsFactory != nullptr)
        m_ImguiWindowsFactory->RenderWindows();

    glfwPollEvents();
}

bool EditorWindow::IsOpen() const
{
    if (m_Window == nullptr)
        return false;
    return !glfwWindowShouldClose(m_Window.get());
}

int EditorWindow::GetWidth() const
{
    return m_Width;
}

int EditorWindow::GetHeight() const
{
    return m_Height;
}

bool EditorWindow::IsVSync() const
{
    return m_VSync;
}

bool EditorWindow::IsFullscreen() const
{
    return m_FullsScreen;
}

bool EditorWindow::IsVisible() const
{
    return m_Visible;
}

bool EditorWindow::IsMouseCursorVisible() const
{
    return m_MouseCursorVisible;
}

MathLib::HVector2 EditorWindow::GetPosition() const
{
    if (m_Window == nullptr)
        return MathLib::HVector2();
    int x, y;
    glfwGetWindowPos(m_Window.get(), &x, &y);
    return MathLib::HVector2(x, y);
}

void EditorWindow::BindRenderSystem(SharedPtr<IRenderSystem> &renderSystem)
{
    m_RenderSystem = renderSystem;
    m_RenderSystem->SetWindowHandle(m_Window.get());
    if (m_ImguiWindowsFactory != nullptr)
        m_ImguiWindowsFactory->ReSet(renderSystem->GetRenderSystemType());
}

RenderSystemType EditorWindow::GetRenderSystemType() const
{
    if (m_RenderSystem)
        return m_RenderSystem->GetRenderSystemType();
    return RenderSystemType::UnKnown;
}