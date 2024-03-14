#include "pch.h"
#include "RenderSystem/RenderWindow.h"

RenderWindow::RenderWindow()
    : m_RenderSystem(nullptr),
      m_Window(nullptr),
      m_Title(""),
      m_Width(800),
      m_Height(600),
      m_VSync(true),
      m_FullsScreen(false),
      m_Visible(true),
      m_MouseCursorVisible(true),
      m_ClearColor(Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)),
      m_Position(Eigen::Vector2f(0.0f, 0.0f))
{
}

RenderWindow::~RenderWindow()
{
    DestroyRenderWindow();
}

void RenderWindow::CreateRenderWindow(const std::string &title, int width, int height)
{
    m_Title = title;
    m_Width = width;
    m_Height = height;
    try
    {
        HASSERT_LOG(SDL_Init(SDL_INIT_VIDEO) == 0, "SDL_Init failed");
        m_Window = std::shared_ptr<SDL_Window>(SDL_CreateWindow(m_Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
        HASSERT_LOG(m_Window != nullptr, "SDL_CreateWindow failed");
    }
    catch (const std::exception &e)
    {
        HLOG_ERROR(e.what());
    }
}

void RenderWindow::DestroyRenderWindow()
{
    if (m_Window != nullptr)
    {
        SDL_DestroyWindow(m_Window.get());
        m_Window = nullptr;
    }
    SDL_Quit();
}

void RenderWindow::SetTitle(const std::string &title)
{
    if (m_Window == nullptr)
        return;
    m_Title = title;
    SDL_SetWindowTitle(m_Window.get(), m_Title.c_str());
}

void RenderWindow::SetSize(int width, int height)
{
    if (m_Window == nullptr)
        return;
    m_Width = width;
    m_Height = height;
    SDL_SetWindowSize(m_Window.get(), m_Width, m_Height);
}

void RenderWindow::SetVSync(bool vsync)
{
    if (m_Window == nullptr)
        return;
    m_VSync = vsync;
    SDL_GL_SetSwapInterval(m_VSync ? 1 : 0);
}

void RenderWindow::SetFullscreen(bool fullscreen)
{
    if (m_Window == nullptr)
        return;
    m_FullsScreen = fullscreen;
    SDL_SetWindowFullscreen(m_Window.get(), m_FullsScreen ? SDL_WINDOW_FULLSCREEN : 0);
}

void RenderWindow::SetVisible(bool visible)
{
    if (m_Window == nullptr)
        return;
    m_Visible = visible;
    SDL_ShowWindow(m_Window.get());
}

void RenderWindow::SetMouseCursorVisible(bool visible)
{
    if (m_Window == nullptr)
        return;
    m_MouseCursorVisible = visible;
    SDL_ShowCursor(m_MouseCursorVisible ? 1 : 0);
}

void RenderWindow::SetPosition(int x, int y)
{
    if (m_Window == nullptr)
        return;
    m_Position = Eigen::Vector2f(x, y);
}

void RenderWindow::SetIcon(const std::string &iconPath)
{
    if (m_Window == nullptr)
        return;
}

void RenderWindow::SetClearColor(const Eigen::Vector4f &color)
{
    m_ClearColor = color;
}

void RenderWindow::Clear()
{
    if (m_RenderSystem)
        m_RenderSystem->Clear(m_ClearColor);

    if (m_Window == nullptr)
        return;
    SDL_FillRect(SDL_GetWindowSurface(m_Window.get()), nullptr, SDL_MapRGB(SDL_GetWindowSurface(m_Window.get())->format, 125, 125, 0));
}

void RenderWindow::Display()
{
    if (m_Window == nullptr)
        return;
    if (m_RenderSystem)
    {
        m_RenderSystem->BeginFrame();
        m_RenderSystem->EndFrame();
    }
    SDL_UpdateWindowSurface(m_Window.get());
}

bool RenderWindow::IsOpen() const
{
    if (m_Window == nullptr)
        return false;
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        // 用户请求退出
        if (e.type == SDL_QUIT)
        {
            return false;
        }
    }
    return true;
}

int RenderWindow::GetWidth() const
{
    return m_Width;
}

int RenderWindow::GetHeight() const
{
    return m_Height;
}

bool RenderWindow::IsVSync() const
{
    return m_VSync;
}

bool RenderWindow::IsFullscreen() const
{
    return m_FullsScreen;
}

bool RenderWindow::IsVisible() const
{
    return m_Visible;
}

bool RenderWindow::IsMouseCursorVisible() const
{
    return m_MouseCursorVisible;
}

Eigen::Vector2f RenderWindow::GetPosition() const
{
    if (m_Window == nullptr)
        return Eigen::Vector2f();
    int x, y;
    SDL_GetWindowPosition(m_Window.get(), &x, &y);
    return Eigen::Vector2f(x, y);
}

void RenderWindow::BindRenderSystem(std::shared_ptr<IRenderSystem> &renderSystem)
{
    m_RenderSystem = renderSystem;
}