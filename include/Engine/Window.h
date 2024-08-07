#pragma once
#include "Common/pch.h"
#include "Engine/EngineInterface.h"

#define EDITOR_MODE

class ImguiWindowsFactory;
struct GLFWwindow;
struct SDL_Renderer;
class EditorWindow : virtual public IWindow
{
public:
    EditorWindow();
    ~EditorWindow();

public:
    void CreateRenderWindow(const std::string &title, int width, int height) override;
    void DestroyRenderWindow() override;
    void SetTitle(const std::string &title) override;
    std::string GetTitle() const { return m_Title; }
    void SetSize(int width, int height) override;
    void SetVSync(bool vsync) override;
    void SetFullscreen(bool fullscreen) override;
    void SetVisible(bool visible) override;
    void SetMouseCursorVisible(bool visible) override;
    void SetPosition(int x, int y) override;
    void SetIcon(const std::string &iconPath) override;
    void SetClearColor(const MathLib::HVector4 &color) override;
    void Clear() override;
    void Display() override;
    bool IsOpen() const override;
    int GetWidth() const override;
    int GetHeight() const override;
    bool IsVSync() const override;
    bool IsFullscreen() const override;
    bool IsVisible() const override;
    bool IsMouseCursorVisible() const override;
    MathLib::HVector2 GetPosition() const override;
    void BindRenderSystem(SharedPtr<IRenderSystem> &renderSystem) override;

public:
    RenderSystemType GetRenderSystemType() const;

private:
    SharedPtr<IRenderSystem> m_RenderSystem;
    SharedPtr<GLFWwindow> m_Window;
    SharedPtr<ImguiWindowsFactory> m_ImguiWindowsFactory;
    std::string m_Title;
    int m_Width;
    int m_Height;
    bool m_VSync;
    bool m_FullsScreen;
    bool m_Visible;
    bool m_MouseCursorVisible;
    MathLib::HVector4 m_ClearColor;
    MathLib::HVector2 m_Position;
};

#if MODULE_TEST
inline bool TestEditorWindow()
{
    EditorWindow window;
    window.CreateRenderWindow("Test", 800, 600);
    window.SetVSync(true);
    window.SetFullscreen(false);
    window.SetVisible(true);
    window.SetMouseCursorVisible(true);
    window.SetPosition(100, 100);
    window.SetIcon("icon.png");
    window.SetClearColor(MathLib::HVector4(0.0f, 0.0f, 0.0f, 1.0f));
    window.Clear();
    window.Display();
    bool isOpen = window.IsOpen();
    HLOG_INFO("Window Is Open: %d\n", isOpen);
    int width = window.GetWidth();
    HLOG_INFO("Window Width: %d\n", width);
    int height = window.GetHeight();
    HLOG_INFO("Window Height: %d\n", height);
    bool vsync = window.IsVSync();
    HLOG_INFO("Window VSync: %d\n", vsync);
    bool fullscreen = window.IsFullscreen();
    HLOG_INFO("Window Is Fullscreen: %d\n", fullscreen);
    bool visible = window.IsVisible();
    HLOG_INFO("Window Is Visible: %d\n", visible);
    bool mouseCursorVisible = window.IsMouseCursorVisible();
    HLOG_INFO("Window Mouse Cursor Visible: %d\n", mouseCursorVisible);
    MathLib::HVector2 position = window.GetPosition();
    HLOG_INFO("Window Position: (%f, %f)\n", position.x(), position.y());
    while (window.IsOpen())
    {
        window.Clear();
        window.Display();
        // Vector2f position = window.GetPosition();
        // printf("Position: (%f, %f)\n", position.x(), position.y());
    }
    window.DestroyRenderWindow();
    return true;
}
#endif