#include "PhysicsSystem.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Engine/Engine.h"
#include "PhysicsRenderer.h"
#include "BroadPhaseLayerImpl.h"
#include "Scene/PhysicsBody3D.h"

namespace bl {

PhysicsSystem::PhysicsSystem(Engine& engine)
    : System(engine)
{
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    
    JPH::RegisterTypes();
    _tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
    _jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

    _physicsSystem.Init(maxBodies, maxBodyMutexes, maxBodyPairs, maxContactConstraints,
        _broadPhaseLayerImpl, _objectVsBroadPhaseLayerFilterImpl, _objectLayerPairFilterImpl);

    _physicsSystem.SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

    _physicsRenderer = std::make_unique<PhysicsRenderer>(engine.GetRenderer());
}

bool PhysicsSystem::Update(float deltaTime, std::function<void()> update)
{

    const float fixedTimeStep = 1.0f / 60.0f;
    _accumulator += deltaTime;
    _accumulator = std::clamp(_accumulator, 0.0f, 2.0f * fixedTimeStep);

    bool physUpdate = false;
    while (_accumulator >= fixedTimeStep)
    {
        physFrameCounter.BeginFrame();

        _physicsSystem.Update(fixedTimeStep, 3, _tempAllocator.get(), _jobSystem.get());
        physUpdate = true;
        _accumulator -= fixedTimeStep;

        update();

        physFrameCounter.EndFrame();
        // rootNode->PhysicsUpdate(fixedTimeStep);
    }

    _interpolationFraction = _accumulator / fixedTimeStep;


    return physUpdate;
}

void PhysicsSystem::InterpolateBodies(float alpha)
{
    JPH::BodyIDVector bodyIDs;
    _physicsSystem.GetActiveBodies(JPH::EBodyType::RigidBody, bodyIDs);

    // float alpha = deltaTime / timeStep;

    for (const JPH::BodyID& id : bodyIDs)
    {
        auto& bodyInterface = _physicsSystem.GetBodyInterface();

        JPH::Vec3 pos;
        JPH::Quat rot;

        bodyInterface.GetPositionAndRotation(id, pos, rot);
        uint64_t userData = bodyInterface.GetUserData(id);

        // PhysicsBody3D* physBody = reinterpret_cast<PhysicsBody3D*>(static_cast<uintptr_t>(userData));

        glm::vec3 newPosition;
        glm::quat newRotation;
        pos.StoreFloat3(reinterpret_cast<JPH::Float3*>(&newPosition));
        rot.StoreFloat4(reinterpret_cast<JPH::Float4*>(&newRotation));

        //physBody->SetWorldPosition(newPosition * alpha + physBody->GetPreviousPosition() * (1 - alpha));
        //physBody->SetWorldRotation(glm::slerp(physBody->GetPreviousRotation(), newRotation, alpha));

        Print::Debug("{}", alpha);

        //physBody->SetPreviousPosition(newPosition);
        //physBody->SetPreviousRotation(newRotation);
    }
}

JPH::PhysicsSystem& PhysicsSystem::GetJolt()
{
    return _physicsSystem;
}

void PhysicsSystem::Draw()
{
    JPH::BodyManager::DrawSettings drawSettings = {};
    drawSettings.mDrawShapeWireframe = true;
    drawSettings.mDrawWorldTransform = true;
    drawSettings.mDrawVelocity = true;
    _physicsSystem.DrawBodies(drawSettings, _physicsRenderer.get());
}

PhysicsRenderer* PhysicsSystem::GetPhysicsRenderer()
{
    return _physicsRenderer.get();
}

float PhysicsSystem::GetPhysicsInterpolationFraction()
{
    return _interpolationFraction;
}

}
