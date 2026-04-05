#pragma once

#include "Resource.h"
#include <Jolt/Jolt.h>
#include 

namespace bl {

class PhysicsShape : public Resource
{
protected:
    PhysicsShape();
public:
    virtual ~PhysicsShape() = 0;
};

} // namespace bl