#pragma once

#include "Node3D.h"

namespace bl {

class RigidBody3D : public Node3D {
public:
    RigidBody3D();
    ~RigidBody3D() = default;
};

} // namespace bl