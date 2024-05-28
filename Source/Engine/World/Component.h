#pragma once

#include "Precompiled.h"

namespace bl
{

/** @brief Base class for every component. */
class Component
{
public:
    Component() = default;
    ~Component() = default;

    virtual void Update() {}
};

} // namespace bl