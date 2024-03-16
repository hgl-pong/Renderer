#pragma once
#include "pch.h"
#include "RenderSystem/RenderSystemInterface.h"

class ImguiWindowsFactory;
class RenderWindow
{
public:
    RenderWindow();
    ~RenderWindow();

public:
    void CreateRenderWindow(const std::string &title, int width, int height);
    void DestroyRenderWindow();
    void SetTitle(const std::string &title);
    void SetSize(int width, int height);
    void SetVSync(bool vsync);
    void SetFullscreen(bool fullscreen);
    void SetVisible(bool visible);
    void SetMouseCursorVisible(bool visible);
    void SetPosition(int x, int y);
    void SetIcon(const std::string &iconPath);
    void SetClearColor(const Eigen::Vector4f &color);
    void Clear();
    void Display();
    bool IsOpen() const;
    int GetWidth() const;
    int GetHeight() const;
    bool IsVSync() const;
    bool IsFullscreen() const;
    bool IsVisible() const;
    bool IsMouseCursorVisible() const;
    Eigen::Vector2f GetPosition() const;
    void BindRenderSystem(std::shared_ptr<IRenderSystem> &renderSystem);

public:
private:
    std::shared_ptr<IRenderSystem> m_RenderSystem;
    std::shared_ptr<SDL_Window> m_Window;
    std::shared_ptr<SDL_Renderer> m_Renderer;
    std::shared_ptr<ImguiWindowsFactory> m_ImguiWindowsFactory;
    std::string m_Title;
    int m_Width;
    int m_Height;
    bool m_VSync;
    bool m_FullsScreen;
    bool m_Visible;
    bool m_MouseCursorVisible;
    Eigen::Vector4f m_ClearColor;
    Eigen::Vector2f m_Position;
};