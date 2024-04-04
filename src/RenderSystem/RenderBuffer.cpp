#include "Common/pch.h"
#include "RenderBuffer.h"

VKRenderBuffer::~VKRenderBuffer()
{
    VK_RELEASE(m_Buffer, vkDestroyBuffer, m_Device);
    VK_RELEASE(m_BufferMemory, vkFreeMemory, m_Device);
}

void VKRenderBuffer::_CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    HASSERT_LOG(vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) == VK_SUCCESS, "Failed to create buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    //allocInfo.memoryTypeIndex = _FindMemoryType(memRequirements.memoryTypeBits, properties);

    HASSERT_LOG(vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) == VK_SUCCESS, "Failed to allocate buffer memory!");

    vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
}
