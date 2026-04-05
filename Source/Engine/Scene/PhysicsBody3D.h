#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "Node3D.h"
#include "Physics/ObjectLayers.h"

namespace bl {

class PhysicsBody3D : public Node3D {
    OBJECT_BOILER(PhysicsBody3D, Node3D)

    JPH::BodyID _bodyId;
    JPH::Shape* _shape = nullptr;
    float _friction = 0.5f;
    float _mass = 1.0f;
    float _restitution = 0.0f;
    JPH::ObjectLayer _objectLayer = ObjectLayers::MOVABLE;
    JPH::EMotionType _motionType = JPH::EMotionType::Dynamic;
    glm::vec3 _prevPosition;
    glm::quat _prevRotation;
    glm::vec3 _currPosition;
    glm::quat _currRotation;
    glm::vec3 _currLinVel;
    glm::vec3 _prevLinVel;

public:
    PhysicsBody3D(Engine& engine);
    PhysicsBody3D(const PhysicsBody3D& copy);
    ~PhysicsBody3D();

    virtual void Update(float deltaTime) override;
    virtual void PhysicsUpdate() override;

    void SetVelocity(const glm::vec3& velocity);
    void SetFriction(float friction);
    void SetMass(float mass);
    void SetRestitution(float restitution);
    void SetObjectLayer(JPH::ObjectLayer objectLayer);
    void SetMotionType(JPH::EMotionType motionType);
    glm::vec3 GetVelocity();
    float GetFriction();
    float GetMass();
    float GetRestitution();
    JPH::ObjectLayer GetObjectLayer();
    JPH::EMotionType GetMotionType();
    void SetDOF(bool allowTranslationX, bool allowTranslationY, bool allowTranslationZ, bool allowRotationX, bool allowRotationY, bool allowRotationZ);
    void SetShape(JPH::Shape* shape);

    void ApplyImpulse(const glm::vec3& impulse);
    void ApplyForce(const glm::vec3& force);

    void ResetBody();
    JPH::BodyID GetBodyID() const;

    static void RegisterClass(ClassDB& db);
};

} // namespace bl