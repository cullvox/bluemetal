#include "Math/Vector2.hpp"
#include <cmath>

namespace bl
{


//===========================//
/* Vector2i */
//===========================//

Vector2i::Vector2i() noexcept
    : x(0), y(0)
{
}

Vector2i::Vector2i(long x, long y) noexcept
    : x(x), y(y)
{
}

Vector2i::Vector2i(long xy) noexcept
    : x(xy), y(xy)
{
}

Vector2i::Vector2i(const Vector2i& rhs) noexcept
    : x(rhs.x), y(rhs.y)
{
}

Vector2i& Vector2i::operator=(const Vector2i& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

Vector2i& Vector2i::operator+(const Vector2i& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2i& Vector2i::operator-(const Vector2i& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2i& Vector2i::operator*(const Vector2i& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2i& Vector2i::operator/(const Vector2i& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

long Vector2i::magnitude() const noexcept
{
    return std::sqrtl(x*x + y*y);
}

long Vector2i::distance(const Vector2i& other) const noexcept
{
    const long xm = (other.x - x);
    const long ym = (other.y - y);
    return std::sqrt(xm*xm + ym*ym);
}

//===========================//
/* Vector2f */
//===========================//

Vector2f::Vector2f() noexcept
    : x(0), y(0)
{
}

Vector2f::Vector2f(float x, float y) noexcept
    : x(x), y(y)
{
}

Vector2f::Vector2f(float xy) noexcept
    : x(xy), y(xy)
{
}

Vector2f& Vector2f::operator=(const Vector2f& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

Vector2f& Vector2f::operator+(const Vector2f& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2f& Vector2f::operator-(const Vector2f& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2f& Vector2f::operator*(const Vector2f& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2f& Vector2f::operator/(const Vector2f& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

float Vector2f::magnitude() const noexcept
{
    return std::sqrt(x*x + y*y);
}

float Vector2f::distance(const Vector2f& other) const noexcept
{
    const float xm = (other.x - x);
    const float ym = (other.y - y);
    return std::sqrt(xm*xm + ym*ym);
}

//===========================//
/* Extent2D */
//===========================//

Extent2D::Extent2D()
    : width(0), height(0)
{
}

Extent2D::Extent2D(unsigned long width, unsigned long height)
    : width(width), height(height)
{
}

Extent2D::Extent2D(unsigned long widthHeight)
    : width(widthHeight), height(widthHeight)
{
}

} /* namespace bl */