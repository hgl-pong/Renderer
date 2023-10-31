#pragma once
#include "physx/PxScene.h"
#include <set>
class Actor;
class Scene
{
public:
    Scene();
    ~Scene();

    void Init();
    void Update(float dt);
    void Shutdown();

    physx::PxScene *GetPhysicsScene() { return m_scene; }

    void AddActor(Actor *actor);
    void RemoveActor(Actor *actor);

private:
    physx::PxScene *m_scene = nullptr;
    std::set<Actor *> m_actors;
};