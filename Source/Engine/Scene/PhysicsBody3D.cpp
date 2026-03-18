#include "PhysicsBody3D.h"
#include "Engine/Engine.h"
#include "Physics/PhysicsSystem.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>


namespace bl {

PhysicsBody3D::PhysicsBody3D(Engine& engine)
    : Node3D(engine)
{
    _currPosition = {};
    _currRotation = {};
}

PhysicsBody3D::PhysicsBody3D(const PhysicsBody3D& rhs)
    : Node3D(rhs)
    , _shape(rhs._shape)
    , _objectLayer(rhs._objectLayer)
    , _motionType(rhs._motionType)
{
    ResetBody();
}

PhysicsBody3D::~PhysicsBody3D()
{
    if (!_bodyId.IsInvalid()) {
        auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
        bodyInterface.RemoveBody(_bodyId);
        bodyInterface.DestroyBody(_bodyId);
    }
}

void PhysicsBody3D::Update(float deltaTime)
{
    Node3D::Update(deltaTime);

    float alpha = GetEngine().GetPhysics().GetPhysicsInterpolationFraction();

    bool interp = true;

    if (interp)
    {
        auto prevPos = GetPosition();
        SetPosition(prevPos + ((_currPosition - prevPos)) * alpha);
        SetRotation(glm::slerp(GetRotation(), _currRotation, alpha));
    }
    else
    {
        SetPosition(_currPosition);
        SetRotation(_currRotation);
    }
}

void PhysicsBody3D::PhysicsUpdate()
{
    Node3D::PhysicsUpdate();

    // Update the body's transform to match the node's transform
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();

    JPH::Vec3 positionVec{};
    JPH::Quat rotationQuat{};
    bodyInterface.GetPositionAndRotation(_bodyId, positionVec, rotationQuat);

    _currPosition = glm::vec3(positionVec.GetX(), positionVec.GetY(), positionVec.GetZ());
    _currRotation = glm::quat(rotationQuat.GetW(), rotationQuat.GetX(), rotationQuat.GetY(), rotationQuat.GetZ());
}

glm::vec3 PhysicsBody3D::GetVelocity()
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    JPH::Vec3 velocity = bodyInterface.GetLinearVelocity(_bodyId);
    return glm::vec3(velocity.GetX(), velocity.GetY(), velocity.GetZ());
}

void PhysicsBody3D::SetVelocity(const glm::vec3& velocity)
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.SetLinearVelocity(_bodyId, JPH::Vec3(velocity.x, velocity.y, velocity.z));
}

void PhysicsBody3D::ApplyImpulse(const glm::vec3& impulse)
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.AddImpulse(_bodyId, JPH::Vec3(impulse.x, impulse.y, impulse.z));
}

void PhysicsBody3D::ApplyForce(const glm::vec3& force)
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.AddForce(_bodyId, JPH::Vec3(force.x, force.y, force.z));
}

void PhysicsBody3D::SetFriction(float friction)
{
    _friction = friction;
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.SetFriction(_bodyId, friction);
}

void PhysicsBody3D::SetMassProperties(float mass)
{
    _mass = mass;
    JPH::BodyLockWrite lock(GetEngine().GetPhysics().GetJolt().GetBodyLockInterface(), _bodyId);  
    if (lock.Succeeded()) {  
        JPH::Body& body = lock.GetBody();
        body.GetMotionProperties()->ScaleToMass(mass);
    }
}

void PhysicsBody3D::SetRestitution(float restitution)
{
    _restitution = restitution;
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.SetRestitution(_bodyId, restitution);
}

void PhysicsBody3D::ResetBody()
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();

    glm::vec3 position = GetPosition();
    glm::quat rotation = GetRotation();

    JPH::BodyCreationSettings settings;
    settings.SetShape(_shape);
    settings.mPosition = JPH::Vec3(position.x, position.y, position.z);
    settings.mRotation = JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w);
    settings.mObjectLayer = _objectLayer;
    settings.mMotionType = _motionType;
    settings.mFriction = _friction;
    settings.mRestitution = _restitution;
    settings.mMassPropertiesOverride.mMass = _mass;
    settings.mUserData = reinterpret_cast<uintptr_t>(this);

    if (!_bodyId.IsInvalid())
        bodyInterface.RemoveBody(_bodyId);

    _bodyId = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
}

void PhysicsBody3D::SetObjectLayer(JPH::ObjectLayer objectLayer)
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.SetObjectLayer(_bodyId, objectLayer);
}

void PhysicsBody3D::SetMotionType(JPH::EMotionType motionType)
{
    _motionType = motionType;
}

void PhysicsBody3D::SetShape(JPH::Shape* shape)
{
    _shape = shape;
    if (_bodyId.IsInvalid()) {
        return;
    }

    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();
    bodyInterface.SetShape(_bodyId, shape, false, JPH::EActivation::Activate);
}


void PhysicsBody3D::SetDOF(
    bool allowTranslationX, bool allowTranslationY, bool allowTranslationZ,
    bool allowRotationX, bool allowRotationY, bool allowRotationZ)
{
    auto& jolt = GetEngine().GetPhysics().GetJolt();

    JPH::BodyLockWrite lock(jolt.GetBodyLockInterface(), _bodyId);
    if (lock.Succeeded()) // body_id may no longer be valid
    {
        JPH::Body &body = lock.GetBody();

        JPH::SixDOFConstraintSettings settings;

        if (!allowTranslationX) {
            settings.MakeFixedAxis(JPH::SixDOFConstraintSettings::EAxis::TranslationX);
        }

        if (!allowTranslationY) {
            settings.MakeFixedAxis(JPH::SixDOFConstraintSettings::EAxis::TranslationY);
        }

        if (!allowTranslationZ) {
            settings.MakeFixedAxis(JPH::SixDOFConstraintSettings::EAxis::TranslationZ);
        }

        if (!allowRotationX) {
            settings.MakeFixedAxis(JPH::SixDOFConstraintSettings::EAxis::RotationX);
        }

        if (!allowRotationY) {
            settings.MakeFixedAxis(JPH::SixDOFConstraintSettings::EAxis::RotationY);
        }

        if (!allowRotationZ) {
            settings.MakeFixedAxis(JPH::SixDOFConstraintSettings::EAxis::RotationZ);
        }

        JPH::Ref<JPH::SixDOFConstraint> constraint = new JPH::SixDOFConstraint(JPH::Body::sFixedToWorld, body, settings);
        jolt.AddConstraint(constraint);
    }
}


} // namespace bl