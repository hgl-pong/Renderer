#pragma once
#include "physx/PxPhysicsAPI.h"

class PhysicsModule
{
public:
    PhysicsModule();
    ~PhysicsModule();

    void Init();
    void Update(float dt);
    void Shutdown();

    physx::PxPhysics *GetPhysics() { return m_physics; }
    physx::PxScene *GetScene() { return m_scene; }

private:
    physx::PxDefaultAllocator m_allocator;
    physx::PxDefaultErrorCallback m_errorCallback;
    physx::PxFoundation *m_foundation = nullptr;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
    physx::PxScene *m_scene = nullptr;
    physx::PxMaterial *m_material = nullptr;
};