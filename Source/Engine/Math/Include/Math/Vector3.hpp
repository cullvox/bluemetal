#pragma once

#include "Math/Export.h"

namespace bl {

class BLOODLUST_MATH_API Vector3i
{
public:
    Vector3i() noexcept;
    Vector3i(long x, long y, long z) noexcept;
    Vector3i(long xyz) noexcept;
    Vector3i(const Vector3i& rhs) noexcept;

    Vector3i& operator=(const Vector3i& rhs) noexcept;
    Vector3i& operator+(const Vector3i& rhs) noexcept;
    Vector3i& operator-(const Vector3i& rhs) noexcept;
    Vector3i& operator*(const Vector3i& rhs) noexcept;
    Vector3i& operator/(const Vector3i& rhs) noexcept;

    int magnitude() const noexcept;
    int distance(const Vector3i& other) const noexcept;

    int x;
    int y;
    int z;
};

class BLOODLUST_MATH_API Vector3f
{
public:
    Vector3f() noexcept;
    Vector3f(float x, float y, float z) noexcept;
    Vector3f(float xyz) noexcept;

    Vector3f& operator=(const Vector3f& rhs) noexcept;
    Vector3f& operator+(const Vector3f& rhs) noexcept;
    Vector3f& operator-(const Vector3f& rhs) noexcept;
    Vector3f& operator*(const Vector3f& rhs) noexcept;
    Vector3f& operator/(const Vector3f& rhs) noexcept;

    float magnitude() const noexcept;
    float distance(const Vector3f& other) const noexcept;

    float x;
    float y;
    float z;
};

class BLOODLUST_MATH_API Extent3D
{
public:
    Extent3D() = default;
    Extent3D(unsigned int width, unsigned int height, unsigned int depth);
    Extent3D(unsigned int widthHeightDepth);

    unsigned int width;
    unsigned int height;
    unsigned int depth;
};

} // namespace bl