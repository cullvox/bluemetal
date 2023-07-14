#include "Vector4.hpp"

///////////////////
// blVector4i
///////////////////

blVector4i::blVector4i() noexcept
    : x(0), y(0), z(0)
{
}

blVector4i::blVector4i(int x, int y, int z, int w) noexcept
    : x(x), y(y), z(z), w(w)
{
}

blVector4i::blVector4i(int xyzw) noexcept
    : x(xyzw), y(xyzw), z(xyzw)
{
}

blVector4i::blVector4i(const blVector4i& rhs) noexcept
    : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
{
}

blVector4i& blVector4i::operator=(const blVector4i& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

blVector4i& blVector4i::operator+(const blVector4i& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

blVector4i& blVector4i::operator-(const blVector4i& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

blVector4i& blVector4i::operator*(const blVector4i& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

blVector4i& blVector4i::operator/(const blVector4i& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

///////////////////
// Vector4f
///////////////////

blVector4f::blVector4f() noexcept
    : x(0), y(0), z(0), w(0)
{
}

blVector4f::blVector4f(float x, float y, float z, float w) noexcept
    : x(x), y(y), z(z), w(w)
{
}

blVector4f::blVector4f(float xyzw) noexcept
    : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{
}

blVector4f& blVector4f::operator=(const blVector4f& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
}

blVector4f& blVector4f::operator+(const blVector4f& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

blVector4f& blVector4f::operator-(const blVector4f& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

blVector4f& blVector4f::operator*(const blVector4f& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

blVector4f& blVector4f::operator/(const blVector4f& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}