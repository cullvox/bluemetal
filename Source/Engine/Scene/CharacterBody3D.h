#pragma once

#include "PhysicsBody3D.h"

namespace bl {

class CharacterBody3D : public PhysicsBody3D {

public:
    CharacterBody3D(Engine& engine);
    ~CharacterBody3D();

    // Add character body specific methods here

    virtual void Update(float dt) override;

    bool IsGrounded();

};

} // namespace bl