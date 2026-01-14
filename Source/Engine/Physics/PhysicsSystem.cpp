#include "PhysicsSystem.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Engine/Engine.h"
#include "PhysicsRenderer.h"
#include "BroadPhaseLayerImpl.h"

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

void PhysicsSystem::Update(float deltaTime)
{
    _physicsSystem.Update(deltaTime, 3, _tempAllocator.get(), _jobSystem.get());

    JPH::BodyManager::DrawSettings drawSettings = {};
    drawSettings.mDrawShapeWireframe = true;
    drawSettings.mDrawShape = true;
    drawSettings.mDrawWorldTransform = true;
    drawSettings.mDrawVelocity = true;
    _physicsSystem.DrawBodies(drawSettings, _physicsRenderer.get());
}

JPH::PhysicsSystem& PhysicsSystem::GetJolt()
{
    return _physicsSystem;
}

PhysicsRenderer* PhysicsSystem::GetPhysicsRenderer()
{
    return _physicsRenderer.get();
}

}
