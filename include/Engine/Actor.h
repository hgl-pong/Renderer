#pragma once
// #include "physx/PxRigidActor.h"
#include <set>
class Component;
enum class ActorType
{
    Static,
    Dynamic,
};
class Actor
{
public:
    Actor();
    virtual ~Actor();

    void Init();
    void Update(float dt);
    void Shutdown();

    // physx::PxRigidActor *GetPhysicsActor() { return m_actor; }

    void AddComponent(Component *component);
    void RemoveComponent(Component *component);

private:
    // physx::PxRigidActor* m_actor = nullptr;
    std::set<Component *> m_components;
};