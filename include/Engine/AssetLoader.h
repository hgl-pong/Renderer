#pragma once
#include <string>
#include <unordered_map>
#include "Model.h"

template <typename Type>
class AssetLoader
{
public:
    virtual void ReadFile(const std::string &filename, Type &asset) = 0;
};

class ModelLoader : public AssetLoader<Model>
{
public:
    void ReadFile(const std::string &filename, Model &asset) override;

private:
    std::unordered_map<std::string, Model> m_models;
};
