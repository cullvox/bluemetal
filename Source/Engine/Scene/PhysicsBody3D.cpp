#include "PhysicsBody3D.h"
#include "Engine/Engine.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>


namespace bl {

PhysicsBody3D::PhysicsBody3D(Engine& engine)
    : Node3D(engine)
{
}

PhysicsBody3D::PhysicsBody3D(const PhysicsBody3D& copy)
    : Node3D(copy)
{
    _shape = copy._shape;
    _objectLayer = copy._objectLayer;
    _motionType = copy._motionType;

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

PhysicsBody3D* PhysicsBody3D::Clone()
{
    return new PhysicsBody3D(*this);
}

void PhysicsBody3D::Update(float deltaTime)
{
    Node3D::Update(deltaTime);

    // Update the body's transform to match the node's transform
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();

    JPH::Vec3 positionVec{};
    JPH::Quat rotationQuat{};
    bodyInterface.GetPositionAndRotation(_bodyId, positionVec, rotationQuat);

    SetPosition(glm::vec3(positionVec.GetX(), positionVec.GetY(), positionVec.GetZ()));
    SetRotation(glm::quat(rotationQuat.GetW(), rotationQuat.GetX(), rotationQuat.GetY(), rotationQuat.GetZ()));

}

void PhysicsBody3D::ResetBody()
{
    auto& bodyInterface = GetEngine().GetPhysics().GetJolt().GetBodyInterface();

    JPH::BodyCreationSettings settings;
    settings.SetShape(_shape);
    settings.mPosition = JPH::Vec3(position.x, position.y, position.z);
    settings.mRotation = JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w);
    settings.mObjectLayer = _objectLayer;
    settings.mMotionType = _motionType;

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