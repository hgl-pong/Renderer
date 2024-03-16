#pragma once
#include "pch.h"
#include "RenderSystem/RenderSystemInterface.h"

class VKRenderShader : virtual public IRenderShader,public RenderSystemResource
{
public:
    VKRenderShader();
    ~VKRenderShader();

private:
    
};