#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"
#include <vulkan/vulkan.h>

class VKRenderBuffer : virtual public IRenderBuffer
{
public:
	virtual ~VKRenderBuffer();
	virtual bool Init() = 0;

protected:
	virtual void _CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
protected:
	VkDevice m_Device = VK_NULL_HANDLE;
	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
};

template <typename DataBlock>
class VKVertexBuffer : public VKRenderBuffer
{
public:
	virtual ~VKVertexBuffer() override
	{
		VKRenderBuffer::~VKRenderBuffer();
	}
	void Init() override {

	};

private:
	size_t m_DataSize;
	std::vector<DataBlock> m_Data;
	VkVertexInputBindingDescription m_BindingDescription;
	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
};