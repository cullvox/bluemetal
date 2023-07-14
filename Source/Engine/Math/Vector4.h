#pragma once

#include "Export.h"

class BLUEMETAL_API blVector4i
{
public:
    blVector4i();
    blVector4i(int x, int y, int z, int w);
    blVector4i(int xyzw);
    blVector4i(const blVector4i& rhs);

    blVector4i& operator=(const blVector4i& rhs);
    blVector4i& operator+(const blVector4i& rhs);
    blVector4i& operator-(const blVector4i& rhs);
    blVector4i& operator*(const blVector4i& rhs);
    blVector4i& operator/(const blVector4i& rhs);

    int magnitude() const;
    int distance(const blVector4i& other) const;

    int x;
    int y;
    int z;
    int w;
};

class BLUEMETAL_API blVector4f
{
public:
    blVector4f();
    blVector4f(float x, float y, float z, float w);
    blVector4f(float xyzw);

    blVector4f& operator=(const blVector4f& rhs);
    blVector4f& operator+(const blVector4f& rhs);
    blVector4f& operator-(const blVector4f& rhs);
    blVector4f& operator*(const blVector4f& rhs);
    blVector4f& operator/(const blVector4f& rhs);

    float magnitude() const;
    float distance(const blVector4f& other) const;

    float x;
    float y;
    float z;
    float w;
};