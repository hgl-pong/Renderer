#pragma once
#include "Module.h"
#include <vector>
// #include "physx/PxPhysicsAPI.h"
class PhysicsModule : public Module
{
public:
    PhysicsModule();
    virtual ~PhysicsModule();

    virtual void Init();
    virtual void Update(float dt);
    virtual void Shutdown();

    // physx::PxPhysics *GetPhysics();
    // physx::PxScene *CreateScene();
    // physx::PxMaterial *CreateMaterial(float staticFriction, float dynamicFriction, float restitution);
    // std::vector<physx::PxConvexMesh *> GenerateConvexMeshes(std::vector<std::vector<physx::PxVec3>> &meshesVertices,
    //                                                         std::vector<std::vector<physx::PxU32>> &meshesIndices);

private:
    // physx::PxDefaultAllocator m_allocator;
    // physx::PxDefaultErrorCallback m_errorCallback;
    // physx::PxFoundation *m_foundation = nullptr;
    // physx::PxPhysics *m_physics = nullptr;
    // physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
    // physx::PxPvd *m_pvd = nullptr;
    // std::set<physx::PxScene *> m_scenes;
};