#pragma once
#include "pch.h"
#include "RenderSystem/RenderSystem.h"

class VKRenderShader : virtual public IRenderShader
{
public:
    VKRenderShader();
    ~VKRenderShader();

private:
    uint32_t m_ShaderID;
};