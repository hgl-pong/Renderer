#pragma once
class IRenderShader;
class IRenderState;
class IRenderTexture;
class IRenderBuffer;
class IRenderUnit;
class IRenderSystem;

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

enum class RenderTextureUsage
{
    RenderTarget,
    DepthStencil,
    ShaderResource,
    UnorderedAccess,
};

struct RenderTextureDesc
{
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mMipLevels;
    uint32_t mArraySize;
    uint32_t mSampleCount;
    uint32_t mSampleQuality;
    RenderTextureType mType;
    RenderTextureUsage mUsage;
    uint32_t mBindFlags;
    uint32_t mCPUAccessFlags;
    uint32_t mMiscFlags;
    uint32_t mFormat;
    uint32_t mClearFlags;
    MathLib::HVector4 mClearColor;
    float mClearDepth;
    uint32_t mClearStencil;
    bool mIsCubeMap;
    bool mIsDynamic;
    bool mIsAutoGenMips;
    bool mIsSRGB;
    bool mIsMultisample;
    bool mIsUAV;
    bool mIsDepthAsSRV;
    bool mIsRandomWrite;
    bool mIsResolveTarget;
    bool mIsResolveSource;
};

class HAPI IRenderTexture
{
public:
    virtual bool Init(IRenderSystem *renderSystem, const RenderTextureDesc &desc) = 0;
    virtual void GetDesc(RenderTextureDesc *desc) const = 0;
};

enum class RenderBufferType
{
    Vertex,
    Index,
    Constant,
    Structured,
    Raw,
};

enum class RenderBufferUsage
{
    Default,
    Immutable,
    Dynamic,
    Staging,
};

struct RenderBufferDesc
{
    uint32_t mElementSize;
    uint32_t mElementCount;
    RenderBufferType mType;
    RenderBufferUsage mUsage;
    uint32_t mBindFlags;
    uint32_t mCPUAccessFlags;
    uint32_t mMiscFlags;
    uint32_t mStructureByteStride;
    bool mIsUAV;
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
    virtual void SetRenderOrder(uint32_t order) = 0;
    virtual uint32_t GetRenderOrder() const = 0;
    virtual void Render() = 0;
};

class RenderSystemResource
{
public:
    virtual ~RenderSystemResource() {}
    uint32_t GetResourceID() const { return m_ID; }

protected:
    uint32_t m_ID;
};

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
    virtual void Clear(const MathLib::HVector4 &color) = 0;
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

HAPI IRenderSystem *CreateRenderSystem(const RenderSystemType &type);
HAPI IRenderSystem *GetRenderSystem();
HAPI void DestroyRenderSystem(IRenderSystem *pRenderSystem);
