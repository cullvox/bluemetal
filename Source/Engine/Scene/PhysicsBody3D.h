#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "Node3D.h"
#include "Physics/ObjectLayers.h"

namespace bl {

class PhysicsBody3D : public Node3D {
    JPH::BodyID _bodyId;
    JPH::Shape* _shape = nullptr;
    JPH::ObjectLayer _objectLayer = ObjectLayers::MOVABLE;
    JPH::EMotionType _motionType = JPH::EMotionType::Dynamic;

public:
    PhysicsBody3D(Engine& engine);
    PhysicsBody3D(const PhysicsBody3D& copy);
    ~PhysicsBody3D();

    virtual void Update(float deltaTime) override;

    void ResetBody();
    virtual PhysicsBody3D* Clone() override;
    void SetObjectLayer(JPH::ObjectLayer objectLayer);
    void SetMotionType(JPH::EMotionType motionType);
    void SetShape(JPH::Shape* shape);
    JPH::BodyID GetBodyID() const;
};

} // namespace bl