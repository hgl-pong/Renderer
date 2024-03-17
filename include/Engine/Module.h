#pragma once
#include <string>
class Module
{
public:
    virtual void init(){};
    virtual void update(float dt){};
    virtual void shutdown(){};
    virtual ~Module() = default;
    const std::string &GetModuleName() const { return name; }

protected:
    std::string name;
};