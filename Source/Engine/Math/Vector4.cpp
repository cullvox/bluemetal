#include "Vector4.h"

namespace bl {

//===========================
// Vector4i
//===========================

Vector4i::Vector4i()
    : x(0)
    , y(0)
    , z(0)
{
}

Vector4i::Vector4i(int x, int y, int z, int w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

Vector4i::Vector4i(int xyzw)
    : x(xyzw)
    , y(xyzw)
    , z(xyzw)
{
}

Vector4i::Vector4i(const Vector4i& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
    , w(rhs.w)
{
}

Vector4i& Vector4i::operator=(const Vector4i& rhs)
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

Vector4i& Vector4i::operator+(const Vector4i& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

Vector4i& Vector4i::operator-(const Vector4i& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

Vector4i& Vector4i::operator*(const Vector4i& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

Vector4i& Vector4i::operator/(const Vector4i& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

//===========================
// Vector4f
//===========================

Vector4f::Vector4f()
    : x(0)
    , y(0)
    , z(0)
    , w(0)
{
}

Vector4f::Vector4f(float x, float y, float z, float w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

Vector4f::Vector4f(float xyzw)
    : x(xyzw)
    , y(xyzw)
    , z(xyzw)
    , w(xyzw)
{
}

Vector4f& Vector4f::operator=(const Vector4f& rhs)
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

Vector4f& Vector4f::operator+(const Vector4f& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

Vector4f& Vector4f::operator-(const Vector4f& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

Vector4f& Vector4f::operator*(const Vector4f& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

Vector4f& Vector4f::operator/(const Vector4f& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

} // namespace bl