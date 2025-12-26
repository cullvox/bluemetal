#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/ext/vector_int2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Extent.h"
#include "Rect.h"

namespace bl {
inline float Lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

inline float DampLinear(float current, float target, float lambda, float dt)
{
    return Lerp(current, target, glm::clamp(lambda * dt, 0.0f, 1.0f));
}

inline float DampExponential(float current, float target, float lambda, float dt)
{
    return Lerp(current, target, 1.0f - std::exp(-lambda * dt));
}

inline glm::vec3 DampExponential(const glm::vec3& current, const glm::vec3& target, float lambda, float dt)
{
    return glm::vec3 {
        DampExponential(current.x, target.x, lambda, dt),
        DampExponential(current.y, target.y, lambda, dt),
        DampExponential(current.z, target.z, lambda, dt),
    };
}

namespace Math {
    static const inline float Pi = 3.14159265f;
}

} // namespace bl