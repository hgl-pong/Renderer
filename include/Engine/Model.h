#pragma once
#include "Common/pch.h"
#include "Component.h"

class Model : public Component
{
public:
    Model();
    ~Model() override;

    void Init();
    void Update(float dt) override;
    void Shutdown();

private:
    friend class ModelLoader;
    std::vector<Mesh> m_meshes;
};