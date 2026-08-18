#pragma once


#include "Extent.h"
#include "Rect.h"
#include "Conversions.h"

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

