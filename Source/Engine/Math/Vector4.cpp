#include "Vector4.h"

//===========================
// blVector4i
//===========================

blVector4i::blVector4i() 
    : x(0), y(0), z(0)
{
}

blVector4i::blVector4i(int x, int y, int z, int w) 
    : x(x), y(y), z(z), w(w)
{
}

blVector4i::blVector4i(int xyzw) 
    : x(xyzw), y(xyzw), z(xyzw)
{
}

blVector4i::blVector4i(const blVector4i& rhs) 
    : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}

blVector4i& blVector4i::operator=(const blVector4i& rhs)
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

blVector4i& blVector4i::operator+(const blVector4i& rhs) 
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

blVector4i& blVector4i::operator-(const blVector4i& rhs) 
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

blVector4i& blVector4i::operator*(const blVector4i& rhs) 
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

blVector4i& blVector4i::operator/(const blVector4i& rhs) 
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

//===========================
// blVector4f
//===========================

blVector4f::blVector4f() 
    : x(0), y(0), z(0), w(0)
{
}

blVector4f::blVector4f(float x, float y, float z, float w) 
    : x(x), y(y), z(z), w(w)
{
}

blVector4f::blVector4f(float xyzw) 
    : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{
}

blVector4f& blVector4f::operator=(const blVector4f& rhs) 
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

blVector4f& blVector4f::operator+(const blVector4f& rhs) 
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

blVector4f& blVector4f::operator-(const blVector4f& rhs) 
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

blVector4f& blVector4f::operator*(const blVector4f& rhs) 
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

blVector4f& blVector4f::operator/(const blVector4f& rhs) 
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}