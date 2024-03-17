#pragma once
#include "Common/pch.h"
enum class RenderSystemType
{
    Vulkan,
    DirectX11,
    DirectX12,
    OpenGL,
};

class HAPI IRenderSystem
{
public:
    virtual void PreInitialize() = 0;
    virtual void PostInitialize() = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Clear(const Eigen::Vector4f& color) = 0;
    virtual RenderSystemType GetRenderSystemType() const = 0;

    // virtual IRenderShader *CreateShader() = 0;
    // virtual IRenderState *CreateRenderState() = 0;
    // virtual IRenderTexture *CreateTexture() = 0;
    // virtual IRenderBuffer *CreateBuffer() = 0;
    // virtual IRenderUint *CreateUint() = 0;

    // virtual void DestroyShader(IRenderShader *pShader) = 0;
    // virtual void DestroyRenderState(IRenderState *pRenderState) = 0;
    // virtual void DestroyTexture(IRenderTexture *pTexture) = 0;
    // virtual void DestroyBuffer(IRenderBuffer *pBuffer) = 0;
    // virtual void DestroyUint(IRenderUint *pUint) = 0;
};

class HAPI IWindow
{
public:
    virtual void CreateRenderWindow(const std::string& title, int width, int height) = 0;
    virtual void DestroyRenderWindow() = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual void SetSize(int width, int height) = 0;
    virtual void SetVSync(bool vsync) = 0;
    virtual void SetFullscreen(bool fullscreen) = 0;
    virtual void SetVisible(bool visible) = 0;
    virtual void SetMouseCursorVisible(bool visible) = 0;
    virtual void SetPosition(int x, int y) = 0;
    virtual void SetIcon(const std::string& iconPath) = 0;
    virtual void SetClearColor(const Eigen::Vector4f& color) = 0;
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
    virtual void BindRenderSystem(std::shared_ptr<IRenderSystem>& renderSystem) = 0;
};
HAPI IWindow* CreateRenderWindow();