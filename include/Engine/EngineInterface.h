#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"
class HAPI IWindow
{
public:
    virtual void CreateRenderWindow(const std::string &title, int width, int height) = 0;
    virtual void DestroyRenderWindow() = 0;
    virtual void SetTitle(const std::string &title) = 0;
    virtual void SetSize(int width, int height) = 0;
    virtual void SetVSync(bool vsync) = 0;
    virtual void SetFullscreen(bool fullscreen) = 0;
    virtual void SetVisible(bool visible) = 0;
    virtual void SetMouseCursorVisible(bool visible) = 0;
    virtual void SetPosition(int x, int y) = 0;
    virtual void SetIcon(const std::string &iconPath) = 0;
    virtual void SetClearColor(const Eigen::Vector4f &color) = 0;
    virtual void Clear() = 0;
    virtual void Display() = 0;
    virtual bool IsOpen() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual bool IsVSync() const = 0;
    virtual bool IsFullscreen() const = 0;
    virtual bool IsVisible() const = 0;
    virtual bool IsMouseCursorVisible() const = 0;
    virtual Eigen::Vector2f GetPosition() const = 0;
    virtual void BindRenderSystem(std::shared_ptr<IRenderSystem> &renderSystem) = 0;
};
HAPI IWindow *CreateRenderWindow();