#include "Common/pch.h"
#include "Engine/Window.h"
#include <SDL2/SDL.h>
#include <imgui.h>
#include "../RenderSystem/imgui/imgui_impl_sdl2.h"
#include "../RenderSystem/imgui/imgui_impl_sdlrenderer2.h"
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
    ImguiWindowsFactory(std::shared_ptr<SDL_Window> &window, std::shared_ptr<SDL_Renderer> &renderer)
    {
        m_Window = window;
        m_Renderer = renderer;
    }
    void SetUp()
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

        ImGui_ImplSDL2_InitForSDLRenderer(m_Window.get(), m_Renderer.get());
        ImGui_ImplSDLRenderer2_Init(m_Renderer.get());
    }

    void ShutDown()
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ProcessEvents(const SDL_Event *event)
    {
        ImGui_ImplSDL2_ProcessEvent(event);
    }

    void RenderWindows()
    {
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        ImGui::ShowDemoWindow();
        for (auto &window : m_Windows)
        {
            window->Render();
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
        SDL_RenderSetScale(m_Renderer.get(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_RenderPresent(m_Renderer.get());
    }

    ImguiWindow *CreateGameViewWindow()
    {
        for (auto &window : m_Windows)
        {
            if (window->GetType() == ImguiWindowType::GameView)
                return window.get();
        }
        m_Windows.push_back(std::shared_ptr<ImguiWindow>(new ImguiGameViewWindow()));
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
    std::shared_ptr<SDL_Window> m_Window;
    std::shared_ptr<SDL_Renderer> m_Renderer;
    std::vector<std::shared_ptr<ImguiWindow>> m_Windows;
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
      m_ClearColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f)),
      m_Position(Vector2f(0.0f, 0.0f))
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
        HASSERT_LOG(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) == 0, "SDL_Init failed");
        // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        m_Window = std::shared_ptr<SDL_Window>(SDL_CreateWindow(m_Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, window_flags), SDL_DestroyWindow);
        HASSERT_LOG(m_Window != nullptr, "SDL_CreateWindow failed");
        m_Renderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(m_Window.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);
        HASSERT_LOG(m_Renderer != nullptr, "SDL_CreateRenderer failed");
        m_ImguiWindowsFactory = std::shared_ptr<ImguiWindowsFactory>(new ImguiWindowsFactory(m_Window, m_Renderer));
        HASSERT_LOG(m_ImguiWindowsFactory != nullptr, "Create ImguiWindowsFactory failed");
        m_ImguiWindowsFactory->SetUp();
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
        SDL_DestroyWindow(m_Window.get());
        m_Window = nullptr;
    }
    if (m_ImguiWindowsFactory != nullptr)
    {
        m_ImguiWindowsFactory->ShutDown();
        m_ImguiWindowsFactory = nullptr;
    }
    if (m_Renderer != nullptr)
    {
        SDL_DestroyRenderer(m_Renderer.get());
        m_Renderer = nullptr;
    }
    SDL_Quit();
}

void EditorWindow::SetTitle(const std::string &title)
{
    if (m_Window == nullptr)
        return;
    m_Title = title;
    SDL_SetWindowTitle(m_Window.get(), m_Title.c_str());
}

void EditorWindow::SetSize(int width, int height)
{
    if (m_Window == nullptr)
        return;
    m_Width = width;
    m_Height = height;
    SDL_SetWindowSize(m_Window.get(), m_Width, m_Height);
}

void EditorWindow::SetVSync(bool vsync)
{
    if (m_Window == nullptr)
        return;
    m_VSync = vsync;
    SDL_GL_SetSwapInterval(m_VSync ? 1 : 0);
}

void EditorWindow::SetFullscreen(bool fullscreen)
{
    if (m_Window == nullptr)
        return;
    m_FullsScreen = fullscreen;
    SDL_SetWindowFullscreen(m_Window.get(), m_FullsScreen ? SDL_WINDOW_FULLSCREEN : 0);
}

void EditorWindow::SetVisible(bool visible)
{
    if (m_Window == nullptr)
        return;
    m_Visible = visible;
    SDL_ShowWindow(m_Window.get());
}

void EditorWindow::SetMouseCursorVisible(bool visible)
{
    if (m_Window == nullptr)
        return;
    m_MouseCursorVisible = visible;
    SDL_ShowCursor(m_MouseCursorVisible ? 1 : 0);
}

void EditorWindow::SetPosition(int x, int y)
{
    if (m_Window == nullptr)
        return;
    m_Position = Vector2f(x, y);
    SDL_SetWindowPosition(m_Window.get(), x, y);
}

void EditorWindow::SetIcon(const std::string &iconPath)
{
    if (m_Window == nullptr)
        return;
}

void EditorWindow::SetClearColor(const Vector4f &color)
{
    m_ClearColor = color;
}

void EditorWindow::Clear()
{
    if (m_RenderSystem)
        m_RenderSystem->Clear(m_ClearColor);

    if (m_Window == nullptr)
        return;

    SDL_SetRenderDrawColor(m_Renderer.get(), (Uint8)(m_ClearColor[0] * 255),
                           (Uint8)(m_ClearColor[1] * 255), (Uint8)(m_ClearColor[2] * 255),
                           (Uint8)(m_ClearColor[3] * 255));
    SDL_RenderClear(m_Renderer.get());
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
    SDL_UpdateWindowSurface(m_Window.get());
}

bool EditorWindow::IsOpen() const
{
    if (m_Window == nullptr)
        return false;
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (m_ImguiWindowsFactory != nullptr)
            m_ImguiWindowsFactory->ProcessEvents(&e);
        // 用户请求退出
        if (e.type == SDL_QUIT)
        {
            return false;
        }
    }
    return true;
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

Vector2f EditorWindow::GetPosition() const
{
    if (m_Window == nullptr)
        return Vector2f();
    int x, y;
    SDL_GetWindowPosition(m_Window.get(), &x, &y);
    return Vector2f(x, y);
}

void EditorWindow::BindRenderSystem(std::shared_ptr<IRenderSystem> &renderSystem)
{
    m_RenderSystem = renderSystem;
}