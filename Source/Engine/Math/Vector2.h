#pragma once

#include "Precompiled.h"
#include "Export.h"

struct BLUEMETAL_API blVector2i
{
    blVector2i() = default;
    blVector2i(int32_t x, int32_t z);
    blVector2i(int32_t xy);
    blVector2i(const blVector2i& rhs);

    blVector2i& operator=(const blVector2i& rhs);
    blVector2i& operator+(const blVector2i& rhs);
    blVector2i& operator-(const blVector2i& rhs);
    blVector2i& operator*(const blVector2i& rhs);
    blVector2i& operator/(const blVector2i& rhs);

    int32_t magnitude();
    int32_t distance(const blVector2i& other);

    int32_t x;
    int32_t y;
};

struct BLUEMETAL_API blVector2f
{
    blVector2f(float x, float z);
    blVector2f(float xy);

    blVector2f& operator=(const blVector2f& rhs);
    blVector2f& operator+(const blVector2f& rhs);
    blVector2f& operator-(const blVector2f& rhs);
    blVector2f& operator*(const blVector2f& rhs);
    blVector2f& operator/(const blVector2f& rhs);

    float magnitude();
    float distance(const blVector2f& other);

    float x;
    float y;
};

struct BLUEMETAL_API blExtent2D
{
    blExtent2D() = default;
    blExtent2D(uint32_t width, uint32_t height);
    blExtent2D(uint32_t widthHeight);
    blExtent2D(VkExtent2D extent);

    explicit operator VkExtent2D();

    uint32_t width;
    uint32_t height;
};

struct BLUEMETAL_API blRect2D
{
    blRect2D() = default;
    blRect2D(blVector2i offset, blExtent2D extent);
    blRect2D(int32_t offsetX, int32_t offsetY, uint32_t extentX, uint32_t extentY);

    blVector2i offset;
    blExtent2D extent;
};