#pragma once
#include "Common/pch.h"
#include "RenderSystem/RenderSystemInterface.h"

class VKRenderSystem;
template <typename DataType>
class VKRenderTexture : virtual public IRenderTexture, public RenderSystemResource
{
public:
    virtual ~VKRenderTexture()
    {
        VkDevice device = m_RenderSystem->GetDevice();
        VK_RELEASE(m_TextureImageView, vkDestroyImageView, device);
        VK_RELEASE(m_TextureImage, vkDestroyImage, device);
        VK_RELEASE(m_TextureMemory, vkFreeMemory, device);
        m_RenderSystem->ReleaseTextureID(m_ID);
    }
    bool Init(IRenderSystem *renderSystem, const RenderTextureDesc &desc) override
    {
        try
        {
            VKRenderSystem *vkRenderSystem = dynamic_cast<VKRenderSystem *>(renderSystem);
            HASSERT_LOG(vkRenderSystem, "RenderSystem type mismatch");
            m_RenderSystem = std::make_shared<VKRenderSystem>(vkRenderSystem);
            m_Desc = desc;
            m_ID = m_RenderSystem->AcquireTextureID();
            VkDevice device = m_RenderSystem->GetDevice();
        }
        catch (const std::exception &e)
        {
        }
    }

    void GetDesc(RenderTextureDesc *desc) const override
    {
        if (dedsc == nullptr)
        {
            HLOG_WARNING("Trying to get Texture Desc Failed, desc is nullptr");
            return;
        }
        *desc = m_Desc;
    }

    bool BindToPipeline(VkCommandBuffer commandBuffer, uint32_t binding, VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, uint32_t setIndex) const
    {
        return false;
    }

    bool BindToPipeline(VkCommandBuffer commandBuffer, uint32_t binding, VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, uint32_t setIndex, VkImageLayout layout) const
    {
        return false;
    }

private:
private:
    RenderTextureDesc m_Desc;
    std::shared_ptr<VKRenderSystem> m_RenderSystem;
    VkImage m_TextureImage = VK_NULL_HANDLE;
    VkImageView m_TextureImageView = VK_NULL_HANDLE;
    VkDeviceMemory m_TextureMemory = VK_NULL_HANDLE;
};
