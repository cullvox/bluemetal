#pragma once

#include "Export.h"
#include "Precompiled.h"

namespace bl
{

class BLUEMETAL_API Vector3i
{
public:
    Vector3i() noexcept;
    Vector3i(int32_t x, int32_t y, int32_t z) noexcept;
    Vector3i(int32_t xyz) noexcept;
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

class BLUEMETAL_API Vector3f
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

class BLUEMETAL_API Extent3D
{
public:
    Extent3D() = default;
    Extent3D(uint32_t width, uint32_t height, uint32_t depth);
    Extent3D(uint32_t widthHeightDepth);
    Extent3D(VkExtent3D extent);

    explicit operator VkExtent3D();

    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

} // namespace bl