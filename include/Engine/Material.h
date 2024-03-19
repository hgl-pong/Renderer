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
        HEIGHT,
        AMBIENT,
        EMISSIVE,
        METALLIC,
        ROUGHNESS,
        AO,
        TRANSMISSION,
        TRANSMISSION_ROUGHNESS,
        TRANSMISSION_METALLIC,
        TRANSMISSION_NORMAL,
        SHEEN,
        SHEEN_TINT,
        CLEARCOAT,
        CLEARCOAT_ROUGHNESS,
        CLEARCOAT_NORMAL,
        IOR,
        REFLECTION,
        REFRACTION,
        MASK,
        UNKNOWN
    };

public:
    Material();

private:
    friend class MaterialLoader;
    std::string m_Name;
    std::unordered_map<TextureType, IRenderTexture *> m_Textures;
};