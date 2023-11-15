#include "PhysicsModule.h"

PhysicsModule::PhysicsModule()
{
    name = "PhysicsModule";
}

PhysicsModule::~PhysicsModule()
{
}

void PhysicsModule::Init()
{
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
	m_pvd = PxCreatePvd(*m_foundation);
	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);
	PxInitExtensions(*m_physics, m_pvd);
	m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
}

void PhysicsModule::Update(float dt)
{
}

void PhysicsModule::Shutdown()
{
    m_dispatcher->release();
    m_physics->release();
    m_foundation->release();
}

physx::PxPhysics *PhysicsModule::GetPhysics()
{
    return m_physics;
}
