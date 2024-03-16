#pragma once
#include "pch.h"
#include "RenderSystem/RenderSystemInterface.h"

class VKRenderSystem;
template <typename DataType>
class VKRenderTexture : virtual public IRenderTexture, public RenderSystemResource
{
public:
    virtual ~VKRenderTexture()
    {
        VkDevice device = m_RenderSystem->GetDevice();
        VK_RELEASE(m_TextureImageView,vkDestroyImageView,device);
        VK_RELEASE(m_TextureImage,vkDestroyImage,device);
        VK_RELEASE(m_TextureMemory,vkFreeMemory,device);
        m_RenderSystem->ReleaseTextureID(m_ID);
    }
    bool Init(IRenderSystem *renderSystem, uint32_t width, uint32_t height, RenderTextureType type) override
    {
        try
        {
            VKRenderSystem* vkRenderSystem = dynamic_cast<VKRenderSystem*>(renderSystem);
            HASSERT_LOG(vkRenderSystem, "RenderSystem type mismatch");
            m_RenderSystem = std::make_shared<VKRenderSystem>(vkRenderSystem);
            m_Width = width;
            m_Height = height;
            m_Type = type;
            m_ID = m_RenderSystem->AcquireTextureID();
        }
        catch (const std::exception& e)
        {
        }
    }
    uint32_t GetWidth() const override { return m_Width; }
    uint32_t GetHeight() const override { return m_Height; }
    RenderTextureType GetType() const override { return m_Type; }

private:
private:
    RenderTextureType m_Type;
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;

    std::shared_ptr<VKRenderSystem> m_RenderSystem;
    VkImage m_TextureImage = VK_NULL_HANDLE;
    VkImageView m_TextureImageView = VK_NULL_HANDLE;
    VkDeviceMemory m_TextureMemory = VK_NULL_HANDLE;
};
