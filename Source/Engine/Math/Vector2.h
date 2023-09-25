#pragma once

#include "Precompiled.h"
#include "Export.h"

namespace bl
{

struct BLUEMETAL_API Vector2i
{
    Vector2i() = default;
    Vector2i(int32_t x, int32_t z);
    Vector2i(int32_t xy);
    Vector2i(const Vector2i& rhs);

    Vector2i& operator=(const Vector2i& rhs);
    Vector2i& operator+(const Vector2i& rhs);
    Vector2i& operator-(const Vector2i& rhs);
    Vector2i& operator*(const Vector2i& rhs);
    Vector2i& operator/(const Vector2i& rhs);

    int32_t magnitude();
    int32_t distance(const Vector2i& other);

    int32_t x;
    int32_t y;
};

struct BLUEMETAL_API Vector2f
{
    Vector2f(float x, float z);
    Vector2f(float xy);

    Vector2f& operator=(const Vector2f& rhs);
    Vector2f& operator+(const Vector2f& rhs);
    Vector2f& operator-(const Vector2f& rhs);
    Vector2f& operator*(const Vector2f& rhs);
    Vector2f& operator/(const Vector2f& rhs);

    float magnitude();
    float distance(const Vector2f& other);

    float x;
    float y;
};

struct BLUEMETAL_API Extent2D
{
    Extent2D() = default;
    Extent2D(uint32_t width, uint32_t height);
    Extent2D(uint32_t widthHeight);
    Extent2D(VkExtent2D extent);

    explicit operator VkExtent2D();

    uint32_t width;
    uint32_t height;
};

using Offset2D = Vector2i;

struct BLUEMETAL_API Rect2D
{
    Rect2D() = default;
    Rect2D(Vector2i offset, Extent2D extent);
    Rect2D(int32_t offsetX, int32_t offsetY, uint32_t extentX, uint32_t extentY);

    Offset2D offset;
    Extent2D extent;
};

} // namespace bl
