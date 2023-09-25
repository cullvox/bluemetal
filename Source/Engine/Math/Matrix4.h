#pragma once

#include "Precompiled.h"
#include "Export.h"

namespace bl
{

struct BLUEMETAL_API Matrix4
{
    blMatrix4() = default;
    blMatrix4(std::array<float, 16> data);

    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
};

} // namespace bl