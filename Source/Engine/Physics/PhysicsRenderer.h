#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/Core.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include "Graphics/RenderData.h"
#include "Graphics/VulkanMesh.h"

#include <Core/Color.h>

namespace bl {

class Renderer;
class VulkanMaterialInstance;

class PhysicsRenderer : public JPH::DebugRenderer {
    static Color ConvertColor(JPH::Color color);

    class BatchImpl : public JPH::RefTargetVirtual
    {
    public:
		JPH_OVERRIDE_NEW_DELETE

		virtual void AddRef() override  { ++mRefCount; }
		virtual void Release() override	{ if (--mRefCount == 0) delete this; }

        VulkanMesh mesh;

    private:
		JPH::atomic<JPH::uint32>			mRefCount = 0;
    };

    Renderer* _renderer;
    RenderData _rd;
    VulkanMaterialInstance* _material;
    glm::vec3 _cameraPosition;

public:
    PhysicsRenderer(Renderer* renderer);
    virtual ~PhysicsRenderer() override;

    void SetMaterial(VulkanMaterialInstance* material);

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) override;
    virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef &inGeometry, ECullMode inCullMode = ECullMode::CullBackFace, ECastShadow inCastShadow = ECastShadow::On, EDrawMode inDrawMode = EDrawMode::Solid) override;

    virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount) override;
	virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount) override;

    void SetCameraPosition(glm::vec3 position);
    void Reset();
    void WriteInstances();
    void RecordCommands();
};

} // namespace bl
