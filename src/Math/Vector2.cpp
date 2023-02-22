#include "Math/Vector2.hpp"

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

int Vector2i::magnitude() const noexcept
{

}

int Vector2i::distance(const Vector2i& other) const noexcept
{

}

Vector2i Vector2i::cross(const Vector2i& other) const noexcept
{

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

Vector2f::Vector2f(const Vector2f& rhs) noexcept
    : x(rhs.x), y(rhs.y)
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

int Vector2f::magnitude() const noexcept
{

}

int Vector2f::distance(const Vector2f& other) const noexcept
{

}

Vector2f Vector2f::cross(const Vector2f& other) const noexcept
{

}

} /* namespace bl */