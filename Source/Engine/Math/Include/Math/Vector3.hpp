#pragma once

#include "Math/Export.h"

class BLOODLUST_MATH_API blVector3i
{
public:
    blVector3i() noexcept;
    blVector3i(int x, int y, int z) noexcept;
    blVector3i(int xyz) noexcept;
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

class BLOODLUST_MATH_API blVector3f
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

class BLOODLUST_MATH_API blExtent3D
{
public:
    blExtent3D() = default;
    blExtent3D(unsigned int width, unsigned int height, unsigned int depth);
    blExtent3D(unsigned int widthHeightDepth);

    unsigned int width;
    unsigned int height;
    unsigned int depth;
};