#include "PhysicsRenderer.h"

#include "Graphics/Renderer.h"

namespace bl {

Color PhysicsRenderer::ConvertColor(JPH::Color color)
{
    return Color{((float)color.r) / 255.0f, ((float)color.g) / 255.0f, ((float)color.b) / 255.0f, ((float)color.a) / 255.0f};
}

PhysicsRenderer::PhysicsRenderer(Renderer* renderer)
    : _renderer(renderer)
{
    Initialize();
}

PhysicsRenderer::~PhysicsRenderer()
{
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

}