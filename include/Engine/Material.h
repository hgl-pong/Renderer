#pragma once
#include "common/pch.h"
#include "RenderSystemInterface.h"

class Material : virtual public IMaterial
{
private:
    enum class TextureType
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };

public:
    Material();

private:
    friend class MaterialLoader;
    std::string m_Name;
    std::unordered_map<TextureType, IRenderTexture *> m_Textures;
}