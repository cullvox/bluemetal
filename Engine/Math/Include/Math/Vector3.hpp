#pragma once

#include "Core/Export.h"

namespace bl {

class BLOODLUST_API Vector3i
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

    long magnitude() const noexcept;
    long distance(const Vector3i& other) const noexcept;

    long x;
    long y;
    long z;
};

class BLOODLUST_API Vector3f
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

class BLOODLUST_API Extent3D
{
public:
    Extent3D();
    Extent3D(unsigned long width, unsigned long height, unsigned long depth);
    Extent3D(unsigned long widthHeightDepth);

    unsigned long width;
    unsigned long height;
    unsigned long depth;
};

} // namespace bl