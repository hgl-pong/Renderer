#pragma once
enum class ComponentType
{
    MODEL,
};
class Component
{
public:
    virtual ~Component() {}
    virtual void Update(float dt){};
    virtual ComponentType GetType() { return m_type; }

protected:
    ComponentType m_type;
};