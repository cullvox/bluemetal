#pragma once

#include "Precompiled.h"

namespace bl
{

class System
{
public:
    System() = default;
    ~System() = default;

    virtual void update(float deltaTime) {}
    virtual void entityRegistered(Entity e) {}
    virtual void entityUnregistered(Entity e) {}

protected:
    std::bitset<WorldLimits::maxEntityCount> _registered;
};

} // namespace bl
