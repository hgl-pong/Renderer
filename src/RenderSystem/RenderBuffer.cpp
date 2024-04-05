#include "Common/pch.h"
#include "RenderBuffer.h"
#include "RenderSystem.h"
VKRenderBuffer::~VKRenderBuffer()
{
    CALL_VK_RENDER_SYSTEM(DestroyVKBuffer, m_Buffer, m_BufferMemory);
}
