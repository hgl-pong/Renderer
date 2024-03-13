#include "pch.h"
#include "RenderSystem/RenderShader.h"
#include "RenderSystem/RenderUnit.h"
#include "RenderSystem/RenderTexture.h"
#include "RenderSystem/RenderBuffer.h"
#include "RenderSystem/RenderSystem.h"
#include <vulkan/vulkan.h>

class VKRenderSystem : public IRenderSystem
{
public:
    void PreInitialize() override;
    void PostInitialize() override;
    void Shutdown() override;
    void BeginFrame() override;
    void EndFrame() override;

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

    bool _CheckValidationLayerSupport();
    static VkBool32 _DebugMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
private:
    VkInstance m_Instance=VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    VkFormat m_SwapChainImageFormat= VK_FORMAT_UNDEFINED;
    VkExtent2D m_SwapChainExtent{0,0};
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
};

//////////////////////////////////////////////////////////////////////////VKRenderSystem Public//////////////////////////////////////////////////////////////////////////
inline void VKRenderSystem::PreInitialize()
{
    _CreateInstance();
    _SetupDebugMessenger();
    _CreateSurface();
    _PickPhysicalDevice();
    _CreateLogicalDevice();
    _CreateSwapChain();
    _CreateImageViews();
    _CreateRenderPass();
    _CreateGraphicsPipeline();
    _CreateFramebuffers();
    _CreateCommandPool();
    _CreateCommandBuffers();
}

inline void VKRenderSystem::PostInitialize()
{
}

inline void VKRenderSystem::Shutdown()
{
}

inline void VKRenderSystem::BeginFrame()
{
}

inline void VKRenderSystem::EndFrame()
{
}
//////////////////////////////////////////////////////////////////////////VKRenderSystem Private//////////////////////////////////////////////////////////////////////////
inline bool VKRenderSystem::_CreateInstance()
{
    std::vector<const char *> extensions;
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    if (m_bEnableDebugUtils)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Render System";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "HEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (m_bEnableDebugUtils && _CheckValidationLayerSupport())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    return vkCreateInstance(&createInfo, nullptr, &m_Instance) == VK_SUCCESS;
}

inline bool VKRenderSystem::_SetupDebugMessenger()
{
    if (!m_bEnableDebugUtils)
        return true;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = _DebugMessageCallback;
    createInfo.pUserData = nullptr;
    
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
		return func(m_Instance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS;
	else
		return false;
}

inline bool VKRenderSystem::_CreateSurface()
{
    return false;
}

inline bool VKRenderSystem::_PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        return false;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    for (const auto &device : devices)
    {
        // if (_IsDeviceSuitable(device))
        //{
        //     m_PhysicalDevice = device;
        //     break;
        // }
    }

    return m_PhysicalDevice != VK_NULL_HANDLE;
}

inline bool VKRenderSystem::_CreateLogicalDevice()
{
    return false;
}

inline bool VKRenderSystem::_CreateSwapChain()
{
    return false;
}

inline bool VKRenderSystem::_CreateImageViews()
{
    return false;
}

inline bool VKRenderSystem::_CreateRenderPass()
{
    return false;
}

inline bool VKRenderSystem::_CreateGraphicsPipeline()
{
    return false;
}

inline bool VKRenderSystem::_CreateFramebuffers()
{
    return false;
}

inline bool VKRenderSystem::_CreateCommandPool()
{
    return false;
}

inline bool VKRenderSystem::_CreateCommandBuffers()
{
    return false;
}

inline bool VKRenderSystem::_CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : m_ValidationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return true;
    }
    return true;
}

VkBool32 VKRenderSystem::_DebugMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    auto curr_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&curr_time));
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        HLOG_VERBOSE(pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        HLOG_INFO(pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        HLOG_WARNING(pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        HLOG_ERROR(pCallbackData->pMessage);
        break;
    default:
        break;
    }
    return VK_TRUE;
};

inline IRenderSystem *CreateRenderSystem(RenderSystemType type)
{
    return new VKRenderSystem();
}

inline void DestroyRenderSystem(IRenderSystem *pRenderSystem)
{
    delete pRenderSystem;
}