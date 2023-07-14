#pragma once

#include "Export.h"

class BLUEMETAL_API blVector4i
{
public:
    blVector4i() noexcept;
    blVector4i(int x, int y, int z, int w) noexcept;
    blVector4i(int xyzw) noexcept;
    blVector4i(const blVector4i& rhs) noexcept;

    blVector4i& operator=(const blVector4i& rhs) noexcept;
    blVector4i& operator+(const blVector4i& rhs) noexcept;
    blVector4i& operator-(const blVector4i& rhs) noexcept;
    blVector4i& operator*(const blVector4i& rhs) noexcept;
    blVector4i& operator/(const blVector4i& rhs) noexcept;

    int magnitude() const noexcept;
    int distance(const blVector4i& other) const noexcept;

    int x;
    int y;
    int z;
    int w;
};

class BLUEMETAL_API blVector4f
{
public:
    blVector4f() noexcept;
    blVector4f(float x, float y, float z, float w) noexcept;
    blVector4f(float xyzw) noexcept;

    blVector4f& operator=(const blVector4f& rhs) noexcept;
    blVector4f& operator+(const blVector4f& rhs) noexcept;
    blVector4f& operator-(const blVector4f& rhs) noexcept;
    blVector4f& operator*(const blVector4f& rhs) noexcept;
    blVector4f& operator/(const blVector4f& rhs) noexcept;

    float magnitude() const noexcept;
    float distance(const blVector4f& other) const noexcept;

    float x;
    float y;
    float z;
    float w;
};