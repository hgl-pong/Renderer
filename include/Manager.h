#pragma once
template <typename Type>
class Manager
{
public:
    Manager() = default;
    virtual ~Manager() = default;

    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Shutdown() = 0;

    virtual Type *Get(const std::string &name) = 0;
    virtual void Add(const std::string &name, Type *object) = 0;
    virtual void Remove(const std::string &name) = 0;

private:
};