#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "Engine/System.h"
#include "BroadPhaseLayerImpl.h"
#include "ObjectVsBroadPhaseLayerFilterImpl.h"
#include "ObjectLayerPairFilterImpl.h"
#include "Core/FrameCounter.h"

namespace bl {

class PhysicsRenderer;

class PhysicsSystem : public System {
    static constexpr uint32_t maxBodies = 1024;
    static constexpr uint32_t maxBodyMutexes = 0;
    static constexpr uint32_t maxBodyPairs = 1024;
    static constexpr uint32_t maxContactConstraints = 1024;

    BroadPhaseLayerImpl _broadPhaseLayerImpl;
    ObjectVsBroadPhaseLayerFilterImpl _objectVsBroadPhaseLayerFilterImpl;
    ObjectLayerPairFilterImpl _objectLayerPairFilterImpl;
    std::unique_ptr<JPH::TempAllocatorImpl> _tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> _jobSystem;
    JPH::PhysicsSystem _physicsSystem;
    std::unique_ptr<PhysicsRenderer> _physicsRenderer;
    float _accumulator = 0.0f;
    float _interpolationFraction = 0.0f;
    FrameCounter physFrameCounter;

public:
    PhysicsSystem(Engine& engine);
    ~PhysicsSystem() override = default;

    JPH::PhysicsSystem& GetJolt();
    bool Update(float deltaTime, std::function<void()> update);
    void InterpolateBodies(float alpha);
    float GetPhysicsInterpolationFraction();
    void Draw();
    PhysicsRenderer* GetPhysicsRenderer();
    FrameCounter& GetPhysFrameCounter() { return physFrameCounter; };
};

}