
#include "PhysicsRenderer.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterialInstance.h"

namespace bl {

Color PhysicsRenderer::ConvertColor(JPH::Color color)
{
    return Color{((float)color.r) / 255.0f, ((float)color.g) / 255.0f, ((float)color.b) / 255.0f, ((float)color.a) / 255.0f};
}

PhysicsRenderer::PhysicsRenderer(Renderer* renderer)
    : _renderer(renderer)
    , _rd(renderer->GetRenderData())
    , _material(nullptr)
{
    Initialize();
}

PhysicsRenderer::~PhysicsRenderer()
{
}

void PhysicsRenderer::SetMaterial(VulkanMaterialInstance* material)
{
    _material = material;

    _material->SetVector4("material.color", glm::vec4{0.06f, 0.72f, 1.0f, 1.0f });
}

void PhysicsRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    const glm::vec3 a{inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()};
    const glm::vec3 b{inTo.GetX(), inTo.GetY(), inTo.GetZ()};
    const Color color = ConvertColor(inColor);

    _renderer->DrawLine(a, b, 3.0f, color);
}

void PhysicsRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow)
{
    const glm::vec3 a{inV1.GetX(), inV1.GetY(), inV1.GetZ()};
    const glm::vec3 b{inV2.GetX(), inV2.GetY(), inV2.GetZ()};
    const glm::vec3 c{inV3.GetX(), inV3.GetY(), inV3.GetZ()};
    const Color color = ConvertColor(inColor);

    _renderer->DrawTriangle(a, b, c, 3.0f, color);
}

void PhysicsRenderer::DrawText3D(JPH::RVec3Arg, const std::string_view &, JPH::ColorArg, float)
{
    // Can't yet.
}

void PhysicsRenderer::DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef &inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode)
{
    glm::mat4 model;
    inModelMatrix.StoreFloat4x4(reinterpret_cast<JPH::Float4*>(&model));

    const auto& lod = inGeometry->mLODs[0];
    const auto& mesh = static_cast<BatchImpl*>(lod.mTriangleBatch.GetPtr())->mesh;

    _rd.DrawInstance(nullptr, _material, &mesh, model);
}

JPH::DebugRenderer::Batch PhysicsRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount)
{
    std::vector<bl::VertexDebug> vertices(inTriangleCount * 3);

    for (int i = 0; i < inTriangleCount; i++)
    {
        const auto& v0 = inTriangles[i].mV[0];
        const auto& v1 = inTriangles[i].mV[1];
        const auto& v2 = inTriangles[i].mV[2];

        vertices.emplace_back(glm::vec3{v0.mPosition.x, v0.mPosition.y, v0.mPosition.z}, glm::vec3{v0.mColor.r, v0.mColor.g, v0.mColor.b}, 0.0f);
        vertices.emplace_back(glm::vec3{v1.mPosition.x, v1.mPosition.y, v1.mPosition.z}, glm::vec3{v1.mColor.r, v1.mColor.g, v1.mColor.b}, 0.0f);
        vertices.emplace_back(glm::vec3{v2.mPosition.x, v2.mPosition.y, v2.mPosition.z}, glm::vec3{v2.mColor.r, v2.mColor.g, v2.mColor.b}, 0.0f);
    }

    BatchImpl* batch = new BatchImpl();
    batch->mesh = VulkanMesh{_renderer->GetDevice(), std::as_bytes(std::span{vertices})};
    return batch;
}

JPH::DebugRenderer::Batch PhysicsRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount)
{
    std::vector<bl::VertexDebug> vertices(inVertexCount);

    for (int i = 0; i < inVertexCount; i++)
    {
        const auto& v = inVertices[i];
        vertices.emplace_back(glm::vec3{v.mPosition.x, v.mPosition.y, v.mPosition.z}, glm::vec3{v.mColor.r, v.mColor.g, v.mColor.b}, 0.0f);
    }

    BatchImpl* batch = new BatchImpl();
    batch->mesh = VulkanMesh{_renderer->GetDevice(), std::as_bytes(std::span{vertices}), std::span{inIndices, static_cast<std::size_t>(inIndexCount)}};
    return batch;
}


}