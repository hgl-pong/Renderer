#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"
#include <vulkan/vulkan.h>

#define CALL_VK_RENDER_SYSTEM(func,...) \
	do{\
         VKRenderSystem* renderSystem=dynamic_cast<VKRenderSystem*>(GetRenderSystem()); \
        if(renderSystem) \
		 {\
			 renderSystem->func(##__VA_ARGS__); \
		 }\
		 else\
		 {\
			 HLOG_ERROR("RenderSystem is nullptr");\
		 }\
    } while (false);

class RenderSystemResourceIDPool
{
public:
    RenderSystemResourceIDPool()
    {
        m_Available.resize(m_Capacity, true);
    }

    uint32_t AcquireID()
    {
        for (uint32_t i = 0; i < m_Capacity; i++)
        {
            if (m_Available[i])
            {
                m_Available[i] = false;
                return i;
            }
        }
        _Resize(m_Capacity * 2);
        return AcquireID();
    }

    void ReleaseID(uint32_t id)
    {
        m_Available[id] = true;
        _CheckCapacity();
    }

private:
    void _CheckCapacity()
    {
        for (uint32_t i = 0; i < m_Capacity; i++)
        {
            if (m_Available[i] == false)
                return;
        }
        _Resize(m_Capacity / 2);
    }

    void _Resize(uint32_t newCapacity)
    {
        std::vector<bool> newAvailable(newCapacity, true);
        for (uint32_t i = 0; i < m_Capacity; i++)
        {
            newAvailable[i] = m_Available[i];
        }
        m_Available = newAvailable;
        m_Capacity = newCapacity;
    }

private:
    std::vector<bool> m_Available;
    uint32_t m_Capacity = 50;
};

class VKRenderSystem : virtual public IRenderSystem
{
private:
    struct VKQueueFamilyIndices
    {
        uint32_t graphicsFamily = UINT32_MAX;
        uint32_t presentFamily = UINT32_MAX;

        bool IsComplete()
        {
            return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX;
        }
    };

public:
    void PreInitialize() override;
    void PostInitialize() override;
    void Shutdown() override;
    void BeginFrame() override;
    void EndFrame() override;
    void Clear(const Vector4f &color) override;
    RenderSystemType GetRenderSystemType() const override { return RenderSystemType::Vulkan; }

public:
    uint32_t AcquireShaderID() { return m_ShaderIDPool.AcquireID(); }
    void ReleaseShaderID(uint32_t id) { m_ShaderIDPool.ReleaseID(id); }
    uint32_t AcquireRenderStateID() { return m_RenderStateIDPool.AcquireID(); }
    void ReleaseRenderStateID(uint32_t id) { m_RenderStateIDPool.ReleaseID(id); }
    uint32_t AcquireTextureID() { return m_TextureIDPool.AcquireID(); }
    void ReleaseTextureID(uint32_t id) { m_TextureIDPool.ReleaseID(id); }
    uint32_t AcquireBufferID() { return m_BufferIDPool.AcquireID(); }
    void ReleaseBufferID(uint32_t id) { m_BufferIDPool.ReleaseID(id); }

    VkDevice GetDevice() const { return m_Device; }

    bool CreateVKBuffer(const RenderBufferDesc& desc, VkBuffer** buffer, VkDeviceMemory** memory);
    bool DestroyVKBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory);

private:
    bool _CreateInstance();
    bool _SetupDebugMessenger();
    bool _CreateSurface();
    bool _PickPhysicalDevice();
    bool _CreateLogicalDevice();
    bool _CreateSwapChain();
    bool _CreateImageViews();
    bool _CreateRenderPass();
    bool _CreateGraphicsPipeline();
    bool _CreateFramebuffers();
    bool _CreateCommandPool();
    bool _CreateCommandBuffers();
    bool _GetQueueFamilyIndices(VKQueueFamilyIndices &indices);

    bool _CheckValidationLayerSupport();
    static VkBool32 _DebugMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_SwapChainExtent{0, 0};
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
    std::vector<VkFence> m_InFlightFences;
    std::vector<VkFence> m_ImagesInFlight;
    size_t m_CurrentFrame = 0;
    bool m_FramebufferResized = false;

#if _DEBUG
    bool m_bEnableDebugUtils = true;
#else
    bool m_bEnableDebugUtils = false;
#endif
    std::vector<const char *> m_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

    RenderSystemResourceIDPool m_ShaderIDPool;
    RenderSystemResourceIDPool m_RenderStateIDPool;
    RenderSystemResourceIDPool m_TextureIDPool;
    RenderSystemResourceIDPool m_BufferIDPool;
};
