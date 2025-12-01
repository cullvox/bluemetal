#pragma once

#include "Node3D.h"

namespace bl {

class RigidBody3D : public Node3D {
public:
    RigidBody3D(Engine& engine);
    ~RigidBody3D() = default;
};

} // namespace bl