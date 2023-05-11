#pragma once

#include "Core/Precompiled.hpp"
#include "Math/Export.h"

struct BLUEMETAL_MATH_API blVector2i
{
    blVector2i(int32_t x, int32_t z) noexcept;
    blVector2i(int32_t xy) noexcept;
    blVector2i(const blVector2i& rhs) noexcept;

    blVector2i& operator=(const blVector2i& rhs) noexcept;
    blVector2i& operator+(const blVector2i& rhs) noexcept;
    blVector2i& operator-(const blVector2i& rhs) noexcept;
    blVector2i& operator*(const blVector2i& rhs) noexcept;
    blVector2i& operator/(const blVector2i& rhs) noexcept;

    int32_t magnitude() const noexcept;
    int32_t distance(const blVector2i& other) const noexcept;

    int32_t x;
    int32_t y;
};

struct BLUEMETAL_MATH_API blVector2f
{
    blVector2f(float x, float z) noexcept;
    blVector2f(float xy) noexcept;

    blVector2f& operator=(const blVector2f& rhs) noexcept;
    blVector2f& operator+(const blVector2f& rhs) noexcept;
    blVector2f& operator-(const blVector2f& rhs) noexcept;
    blVector2f& operator*(const blVector2f& rhs) noexcept;
    blVector2f& operator/(const blVector2f& rhs) noexcept;

    float magnitude() const noexcept;
    float distance(const blVector2f& other) const noexcept;

    float x;
    float y;
};

struct BLUEMETAL_MATH_API blExtent2D
{
    blExtent2D() = default;
    blExtent2D(uint32_t width, uint32_t height)
        : width(width), height(height)
    {}

    blExtent2D(uint32_t wh)
        : width(wh), height(wh)
    {} 

    uint32_t width;
    uint32_t height;
};

struct BLUEMETAL_MATH_API blRect2D
{
    blRect2D() = default;
    blRect2D(blExtent2D offset, blExtent2D extent)
        : offset(offset), extent(extent)
    {}

    blRect2D(int32_t offsetX, int32_t offsetY, uint32_t extentX, 
            uint32_t extentY)
        : offset(offsetX, offsetY), extent(extentX, extentY) 
    {}

    blExtent2D offset;
    blExtent2D extent;
};