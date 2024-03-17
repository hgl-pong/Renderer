#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"

class VKRenderShader : virtual public IRenderShader, public RenderSystemResource
{
public:
    VKRenderShader();
    ~VKRenderShader();

private:
};