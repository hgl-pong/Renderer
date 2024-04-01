#pragma once
#include "Engine/RenderSystemInterface.h"

class VKRenderBuffer :virtual  public IRenderBuffer
{
public:

};

template<typename DataBlock>
class VKVertexBuffer : public VKRenderBuffer
{
pubic:
private:
	size_t m_DataSize;
	std::vector<DataBlock> m_Data;
	VkVertexInputBindingDescription m_BindingDescription;
	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
};