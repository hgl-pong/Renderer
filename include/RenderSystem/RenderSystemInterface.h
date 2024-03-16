#pragma once
#include "pch.h"
class IRenderSystem;
class IRenderShader;
class IRenderState;
class IRenderTexture;
class IRenderBuffer;
class IRenderUnit;
class IRenderWindow;

enum class RenderSystemType
{
    Vulkan,
    DirectX11,
    DirectX12,
    OpenGL,
};

enum class RenderShaderType
{
    Vertex,
    Pixel,
    Compute,
    Geometry,
    Hull,
    Domain,
};

class HAPI IRenderShader
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

class HAPI IRenderTexture
{
public:
    virtual bool Init(IRenderSystem *renderSystem, uint32_t width, uint32_t height, RenderTextureType type) = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual RenderTextureType GetType() const = 0;
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

enum class RenderUnitType
{
    Fence,
    Event,
    Query,
};

class HAPI IRenderUnit
{
public:
};

class HAPI IRenderSystem
{
public:
    virtual void PreInitialize() = 0;
    virtual void PostInitialize() = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Clear(const Eigen::Vector4f &color) = 0;
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

class RenderSystemResource
{
public:
    virtual ~RenderSystemResource() {}
    uint32_t GetResourceID() const { return m_ID; }

protected:
    uint32_t m_ID;
};

class HAPI IRenderWindow
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

HAPI IRenderSystem *CreateRenderSystem(const RenderSystemType &type);
HAPI void DestroyRenderSystem(IRenderSystem *pRenderSystem);