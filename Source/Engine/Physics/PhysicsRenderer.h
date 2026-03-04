#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/Core.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include <Core/Color.h>

namespace bl {

class Renderer;

class PhysicsRenderer : public JPH::DebugRenderer {
    static Color ConvertColor(JPH::Color color);
    Renderer* _renderer;
public:
    PhysicsRenderer(Renderer* renderer);
    virtual ~PhysicsRenderer() override;

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) override;

    virtual Batch CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) override;
	virtual Batch CreateTriangleBatch(const Vertex *inVertices, int inVertexCount, const uint32 *inIndices, int inIndexCount) override;
};

} // namespace bl
