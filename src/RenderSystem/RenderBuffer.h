#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"
#include <vulkan/vulkan.h>

class VKRenderBuffer : virtual public IRenderBuffer, public RenderSystemResource
{
public:
	virtual ~VKRenderBuffer();
	virtual bool Init() = 0;

protected:
	//VkDevice m_Device = VK_NULL_HANDLE;
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
				// Create buffer
		VkDeviceSize bufferSize = sizeof(DataBlock) * m_Data.size();
	};

private:
	size_t m_DataSize;
	std::vector<DataBlock> m_Data;
	VkVertexInputBindingDescription m_BindingDescription;
	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
};