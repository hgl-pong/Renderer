#pragma once
#include "Common/pch.h"
#include "RenderSystem/RenderSystemInterface.h"

#define EDITOR_MODE

class ImguiWindowsFactory;
class EditorWindow : virtual public IWindow
{
public:
    EditorWindow();
    ~EditorWindow();

public:
    void CreateRenderWindow(const std::string &title, int width, int height)override;
    void DestroyRenderWindow()override;
    void SetTitle(const std::string &title)override;
    void SetSize(int width, int height)override;
    void SetVSync(bool vsync)override;
    void SetFullscreen(bool fullscreen)override;
    void SetVisible(bool visible)override;
    void SetMouseCursorVisible(bool visible)override;
    void SetPosition(int x, int y)override;
    void SetIcon(const std::string &iconPath)override;
    void SetClearColor(const Eigen::Vector4f &color)override;
    void Clear()override;
    void Display()override;
    bool IsOpen() const override;
    int GetWidth() const override;
    int GetHeight() const override;
    bool IsVSync() const override;
    bool IsFullscreen() const override;
    bool IsVisible() const override;
    bool IsMouseCursorVisible() const override;
    Eigen::Vector2f GetPosition() const override;
    void BindRenderSystem(std::shared_ptr<IRenderSystem> &renderSystem)override;

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