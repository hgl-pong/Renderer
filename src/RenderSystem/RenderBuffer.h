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
		_CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferMemory);

		// Copy data
		void* data;
		vkMapMemory(m_Device, m_BufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_Data.data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device, m_BufferMemory);

		// Set binding description
		m_BindingDescription.binding = 0;
		m_BindingDescription.stride = sizeof(DataBlock);
		m_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		// Set attribute descriptions
		m_AttributeDescriptions.resize(1);
		m_AttributeDescriptions[0].binding = 0;
		m_AttributeDescriptions[0].location = 0;
		m_AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_AttributeDescriptions[0].offset = offsetof(DataBlock, position);	
	};

private:
	size_t m_DataSize;
	std::vector<DataBlock> m_Data;
	VkVertexInputBindingDescription m_BindingDescription;
	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
};