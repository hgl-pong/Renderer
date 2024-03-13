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
inline IRenderSystem *CreateRenderSystem(RenderSystemType type);
inline void DestroyRenderSystem(IRenderSystem *pRenderSystem);
