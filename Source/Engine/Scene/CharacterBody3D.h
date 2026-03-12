#pragma once

#include "PhysicsBody3D.h"

namespace bl {

class Orbit3D;
class CharacterBody3D : public PhysicsBody3D {
    Orbit3D* _orbiter;
public:
    CharacterBody3D(Engine& engine);
    ~CharacterBody3D();

    // Add character body specific methods here

    virtual void Update(float dt) override;

    bool IsGrounded();

};

} // namespace bl