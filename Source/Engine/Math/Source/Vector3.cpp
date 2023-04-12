#include "Math/Vector3.hpp"

#include <cmath>

namespace bl
{


//===========================//
/* Vector3i */
//===========================//

Vector3i::Vector3i() noexcept
    : x(0), y(0), z(0)
{
}

Vector3i::Vector3i(long x, long y, long z) noexcept
    : x(x), y(y), z(z)
{
}

Vector3i::Vector3i(long xyz) noexcept
    : x(xyz), y(xyz), z(xyz)
{
}

Vector3i::Vector3i(const Vector3i& rhs) noexcept
    : x(rhs.x), y(rhs.y), z(rhs.z)
{
}

Vector3i& Vector3i::operator=(const Vector3i& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
}

Vector3i& Vector3i::operator+(const Vector3i& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3i& Vector3i::operator-(const Vector3i& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3i& Vector3i::operator*(const Vector3i& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vector3i& Vector3i::operator/(const Vector3i& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

long Vector3i::magnitude() const noexcept
{
    return std::sqrt(x*x + y*y + z*z);
}

long Vector3i::distance(const Vector3i& other) const noexcept
{
    const long xm = (other.x - x);
    const long ym = (other.y - y);
    const long zm = (other.z - z);
    return std::sqrt(xm*xm + ym*ym + zm*zm);
}

//===========================//
/* Vector3f */
//===========================//

Vector3f::Vector3f() noexcept
    : x(0), y(0), z(0)
{
}

Vector3f::Vector3f(float x, float y, float z) noexcept
    : x(x), y(y), z(z)
{
}

Vector3f::Vector3f(float xyz) noexcept
    : x(xyz), y(xyz), z(xyz)
{
}

Vector3f& Vector3f::operator=(const Vector3f& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
}

Vector3f& Vector3f::operator+(const Vector3f& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3f& Vector3f::operator-(const Vector3f& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3f& Vector3f::operator*(const Vector3f& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vector3f& Vector3f::operator/(const Vector3f& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

float Vector3f::magnitude() const noexcept
{
    return std::sqrt(x*x + y*y + z*z);
}

float Vector3f::distance(const Vector3f& other) const noexcept
{
    const float xm = (other.x - x);
    const float ym = (other.y - y);
    const float zm = (other.z - z); 
    return std::sqrt(xm*xm + ym*ym + zm*zm);
}

//===========================//
/* Extent3D */
//===========================//

Extent3D::Extent3D(unsigned long width, unsigned long height, unsigned long depth)
    : width(width), height(height), depth(depth)
{
}

Extent3D::Extent3D(unsigned long widthHeightDepth)
    : width(widthHeightDepth), height(widthHeightDepth), depth(widthHeightDepth)
{
}

} /* namespace bl */