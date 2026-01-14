#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include <Core/Color.h>

namespace bl {

class Renderer;

class PhysicsRenderer : public JPH::DebugRendererSimple {
    static Color ConvertColor(JPH::Color color);
    Renderer* _renderer;
public:
    PhysicsRenderer(Renderer* renderer);
    virtual ~PhysicsRenderer() override;

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) override;
};

} // namespace bl
