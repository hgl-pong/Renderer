#pragma once
#include "pch.h"
enum class RenderShaderType
{
    Vertex,
    Pixel,
    Compute,
    Geometry,
    Hull,
    Domain,
};

class IRenderShader
{
public:
};

enum class RenderStateType
{
    Rasterizer,
    DepthStencil,
    Blend,
    Sampler,
};

class IRenderState
{
public:
};

enum class RenderTextureType
{
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture1DArray,
    Texture2DArray,
    TextureCubeArray,
    Texture2DMultiSample,
    Texture2DMultiSampleArray,
};

class IRenderTexture
{
public:
};
enum class RenderBufferType
{
    Vertex,
    Index,
    Constant,
    Structured,
    Raw,
};

class IRenderBuffer
{
public:
};

enum class RenderUintType
{
    Fence,
    Event,
    Query,
};

class IRenderUint
{
public:
};

class IRenderSystem
{
public:
    virtual void PreInitialize() = 0;
    virtual void PostInitialize() = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Clear(const Eigen::Vector4f &color) = 0;

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

enum class RenderSystemType
{
    Vulkan,
    DirectX11,
    DirectX12,
    OpenGL,
};
class IRenderWindow
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
    virtual void BindRenderSystem(IRenderSystem *renderSystem) = 0;
};

inline IRenderSystem *CreateRenderSystem(RenderSystemType type);
inline void DestroyRenderSystem(IRenderSystem *pRenderSystem);
