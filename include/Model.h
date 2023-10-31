#pragma once
#include "Component.h"
#include <vector>
#include <Types.h>
#include "physx/PxPhysicsAPI.h"
struct Mesh
{
    std::vector<physx::PxVec3> m_vertices;
    std::vector<uint32_t> m_indices;
    Color m_color;
};
class Model : public Component
{
public:
    Model();
    ~Model()override;

    void Init();
    void Update(float dt) override;
    void Shutdown();

private:
    friend class ModelLoader;
    std::vector<Mesh> m_meshes;
};