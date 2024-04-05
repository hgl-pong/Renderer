#include "Common/pch.h"
#include "RenderShader.h"
#include "RenderUnit.h"
#include "RenderTexture.h"
#include "RenderBuffer.h"
#include "RenderSystem.h"

#define HASSERT_VK(x) HASSERT((x) == VK_SUCCESS)
IRenderSystem* singleTonRenderSystem = nullptr;

//////////////////////////////////////////////////////////////////////////VKRenderSystem Public//////////////////////////////////////////////////////////////////////////
inline void VKRenderSystem::PreInitialize()
{
    bool bResult = true;
    try
    {
        HASSERT(_CreateInstance());
        HASSERT(_SetupDebugMessenger());
        HASSERT(_CreateSurface());
        HASSERT(_PickPhysicalDevice());
        HASSERT(_CreateLogicalDevice());
        HASSERT(_CreateSwapChain());
        HASSERT(_CreateImageViews());
        HASSERT(_CreateRenderPass());
        HASSERT(_CreateGraphicsPipeline());
        HASSERT(_CreateFramebuffers());
        HASSERT(_CreateCommandPool());
        HASSERT(_CreateCommandBuffers());
    }
    catch (const std::exception &e)
    {
        bResult = false;
    }
    if (!bResult)
    {
        Shutdown();
    }
}

inline void VKRenderSystem::PostInitialize()
{
}

inline void VKRenderSystem::Shutdown()
{
    if (m_bEnableDebugUtils)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(m_Instance, m_DebugMessenger, nullptr);
    }
    for (auto commandBuffer : m_CommandBuffers)
        vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    for (auto framebuffer : m_SwapChainFramebuffers)
        vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
    vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    for (auto imageView : m_SwapChainImageViews)
        vkDestroyImageView(m_Device, imageView, nullptr);
    for (auto image : m_SwapChainImages)
        vkDestroyImage(m_Device, image, nullptr);
    vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

inline void VKRenderSystem::BeginFrame()
{
    vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(m_Device, 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
}

inline void VKRenderSystem::EndFrame()
{
}

inline void VKRenderSystem::Clear(const Vector4f &color)
{
}

inline bool VKRenderSystem::CreateVKBuffer(const RenderBufferDesc& desc, VkBuffer** buffer, VkDeviceMemory** memory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = desc.mElementCount * desc.mElementSize;
	//bufferInfo.usage = desc.usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	HASSERT_VK(vkCreateBuffer(m_Device, &bufferInfo, nullptr, *buffer));
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, **buffer, &memRequirements);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = 0;
	HASSERT_VK(vkAllocateMemory(m_Device, &allocInfo, nullptr, *memory));
	vkBindBufferMemory(m_Device, **buffer, **memory, 0);
	return true;
}


inline bool VKRenderSystem::DestroyVKBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VK_RELEASE(buffer, vkDestroyBuffer, m_Device);
    VK_RELEASE(bufferMemory, vkFreeMemory, m_Device);
	return false;
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

    auto IsDeviceSuitable = [](VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader;
    };

    for (const auto &device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
            break;
        }
    }

    return m_PhysicalDevice != VK_NULL_HANDLE;
}

inline bool VKRenderSystem::_CreateLogicalDevice()
{
    bool bResult = true;
    VKQueueFamilyIndices indices;
    if (!_GetQueueFamilyIndices(indices))
        return false;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeature{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeature;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
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
    try
    {
        HASSERT_VK(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device));
    }
    catch (const std::exception &e)
    {
        bResult = false;
        return false;
    }
    vkGetDeviceQueue(m_Device, indices.graphicsFamily, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, indices.presentFamily, 0, &m_PresentQueue);
    return bResult;
}

inline bool VKRenderSystem::_CreateSwapChain()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, formats.data());
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data());
    VkSurfaceFormatKHR surfaceFormat = formats[0];
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D extent = {0, 0};
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }
    for (const auto &format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = format;
            break;
        }
    }
    for (const auto &mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = mode;
            break;
        }
    }
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        extent = capabilities.currentExtent;
    }
    else
    {
        int width, height;
        // glfwGetFramebufferSize(m_Window, &width, &height);

        // extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        // extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        // extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    return false;
}

inline bool VKRenderSystem::_CreateImageViews()
{
    m_SwapChainImageViews.resize(m_SwapChainImages.size());

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_SwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_SwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        HASSERT_VK(vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]));
    }
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
    m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {m_SwapChainImageViews[i]};
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_SwapChainExtent.width;
        framebufferInfo.height = m_SwapChainExtent.height;
        framebufferInfo.layers = 1;
        framebufferInfo.layers = 1;

        HASSERT_VK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]));
    }
    return false;
}

inline bool VKRenderSystem::_CreateCommandPool()
{
    VKQueueFamilyIndices indices;
    if (!_GetQueueFamilyIndices(indices))
        return false;
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = indices.graphicsFamily;
    HASSERT_VK(vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool));
    return true;
}

inline bool VKRenderSystem::_CreateCommandBuffers()
{
    m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());
    HASSERT_VK(vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()));
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
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
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

bool VKRenderSystem::_GetQueueFamilyIndices(VKQueueFamilyIndices &indices)
{
    bool bResult = false;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t graphicsFamily = UINT_MAX;
    uint32_t presentFamily = UINT_MAX;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphicsFamily = i;
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupport);
        if (presentSupport)
            presentFamily = i;
        if (graphicsFamily != UINT_MAX && presentFamily != UINT_MAX)
            break;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////RenderSystem Factory//////////////////////////////////////////////////////////////////////////

IRenderSystem *CreateRenderSystem(const RenderSystemType &type)
{
    switch (type)
    {
    case RenderSystemType::Vulkan:
        return new VKRenderSystem();
        break;
    case RenderSystemType::DirectX11:
        return nullptr;
        break;
    case RenderSystemType::DirectX12:
        return nullptr;
        break;
    case RenderSystemType::OpenGL:
        return nullptr;
        break;
    default:
        break;
    }
    return nullptr;
}

IRenderSystem* GetRenderSystem()
{
	return singleTonRenderSystem;
}

void DestroyRenderSystem(IRenderSystem *pRenderSystem)
{
    delete pRenderSystem;
}