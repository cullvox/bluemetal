#pragma once

#include "Export.h"
#include "Precompiled.h"

class BLUEMETAL_API blVector3i
{
public:
    blVector3i() noexcept;
    blVector3i(int32_t x, int32_t y, int32_t z) noexcept;
    blVector3i(int32_t xyz) noexcept;
    blVector3i(const blVector3i& rhs) noexcept;

    blVector3i& operator=(const blVector3i& rhs) noexcept;
    blVector3i& operator+(const blVector3i& rhs) noexcept;
    blVector3i& operator-(const blVector3i& rhs) noexcept;
    blVector3i& operator*(const blVector3i& rhs) noexcept;
    blVector3i& operator/(const blVector3i& rhs) noexcept;

    int magnitude() const noexcept;
    int distance(const blVector3i& other) const noexcept;

    int x;
    int y;
    int z;
};

class BLUEMETAL_API blVector3f
{
public:
    blVector3f() noexcept;
    blVector3f(float x, float y, float z) noexcept;
    blVector3f(float xyz) noexcept;

    blVector3f& operator=(const blVector3f& rhs) noexcept;
    blVector3f& operator+(const blVector3f& rhs) noexcept;
    blVector3f& operator-(const blVector3f& rhs) noexcept;
    blVector3f& operator*(const blVector3f& rhs) noexcept;
    blVector3f& operator/(const blVector3f& rhs) noexcept;

    float magnitude() const noexcept;
    float distance(const blVector3f& other) const noexcept;

    float x;
    float y;
    float z;
};

class BLUEMETAL_API blExtent3D
{
public:
    blExtent3D() = default;
    blExtent3D(uint32_t width, uint32_t height, uint32_t depth);
    blExtent3D(uint32_t widthHeightDepth);
    blExtent3D(VkExtent3D extent);

    explicit operator VkExtent3D();

    uint32_t width;
    uint32_t height;
    uint32_t depth;
};