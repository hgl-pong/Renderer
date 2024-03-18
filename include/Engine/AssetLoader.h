#pragma once
#include "Common/pch.h"
#include "Model.h"

template <typename Type>
class AssetLoader
{
public:
    virtual void ReadFile(const std::string &filename, Type &asset) = 0;
};

class ImageLoader : virtual public AssetLoader<Image>
{
public:
    void ReadFile(const std::string &filename, Image &asset) override;

private:
    std::unordered_map<std::string, Image> m_images;
};

class ModelLoader : virtual public AssetLoader<Model>
{
public:
    void ReadFile(const std::string &filename, Model &asset) override;

private:
    std::unordered_map<std::string, Model> m_models;
};

class MaterialLoader : virtual public AssetLoader<IMaterial>
{
public:
    void ReadFile(const std::string &filename, IMaterial &asset) override;

private:
    std::unordered_map<std::string, IMaterial> m_materials;
};

inline bool TestModelLoader()
{
    ModelLoader loader;
    Model model;
    loader.ReadFile("cube.obj", model);
    return true;
}

inline bool TestImageLoader()
{
    ImageLoader loader;
    Image image;
    loader.ReadFile("atlas.png", image);
    return true;
}
