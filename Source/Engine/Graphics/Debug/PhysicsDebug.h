#pragma once

#include <Jolt/Renderer/DebugRendererSimple.h>

namespace bl {

class PhysicsRender : public JPH::DebugRendererSimple {

public:
    PhysicsRender();
    virtual ~PhysicsRender() override;

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::ECastShadow inCastShadow = JPH::ECastShadow::Off) override;
    

};

} // namespace bl