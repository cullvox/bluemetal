#pragma once

#include "Export.h"

namespace bl
{

class BLUEMETAL_API Vector4i
{
public:
    Vector4i();
    Vector4i(int x, int y, int z, int w);
    Vector4i(int xyzw);
    Vector4i(const Vector4i& rhs);

    Vector4i& operator=(const Vector4i& rhs);
    Vector4i& operator+(const Vector4i& rhs);
    Vector4i& operator-(const Vector4i& rhs);
    Vector4i& operator*(const Vector4i& rhs);
    Vector4i& operator/(const Vector4i& rhs);

    int magnitude() const;
    int distance(const Vector4i& other) const;

    int x;
    int y;
    int z;
    int w;
};

class BLUEMETAL_API Vector4f
{
public:
    Vector4f();
    Vector4f(float x, float y, float z, float w);
    Vector4f(float xyzw);

    Vector4f& operator=(const Vector4f& rhs);
    Vector4f& operator+(const Vector4f& rhs);
    Vector4f& operator-(const Vector4f& rhs);
    Vector4f& operator*(const Vector4f& rhs);
    Vector4f& operator/(const Vector4f& rhs);

    float magnitude() const;
    float distance(const Vector4f& other) const;

    float x;
    float y;
    float z;
    float w;
};

} // namespace bl