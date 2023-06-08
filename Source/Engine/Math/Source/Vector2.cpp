#include "Math/Vector2.hpp"

#include <cmath>


//===========================//
/* blVector2i */
//===========================//

blVector2i::blVector2i(int32_t x, int32_t y) noexcept
    : x(x), y(y)
{
}

blVector2i::blVector2i(int32_t xy) noexcept
    : x(xy), y(xy)
{
}

blVector2i::blVector2i(const blVector2i& rhs) noexcept
    : x(rhs.x), y(rhs.y)
{
}

blVector2i& blVector2i::operator=(const blVector2i& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

blVector2i& blVector2i::operator+(const blVector2i& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

blVector2i& blVector2i::operator-(const blVector2i& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

blVector2i& blVector2i::operator*(const blVector2i& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

blVector2i& blVector2i::operator/(const blVector2i& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

int32_t blVector2i::magnitude() const noexcept
{
    return (int32_t)std::sqrt(x*x + y*y);
}

int32_t blVector2i::distance(const blVector2i& other) const noexcept
{
    const int32_t xm = (other.x - x);
    const int32_t ym = (other.y - y);
    return (int32_t)std::sqrt(xm*xm + ym*ym);
}

//===========================//
/* blVector2f */
//===========================//


blVector2f::blVector2f(float x, float y) noexcept
    : x(x), y(y)
{
}

blVector2f::blVector2f(float xy) noexcept
    : x(xy), y(xy)
{
}

blVector2f& blVector2f::operator=(const blVector2f& rhs) noexcept
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

blVector2f& blVector2f::operator+(const blVector2f& rhs) noexcept
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

blVector2f& blVector2f::operator-(const blVector2f& rhs) noexcept
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

blVector2f& blVector2f::operator*(const blVector2f& rhs) noexcept
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

blVector2f& blVector2f::operator/(const blVector2f& rhs) noexcept
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

float blVector2f::magnitude() const noexcept
{
    return std::sqrt(x*x + y*y);
}

float blVector2f::distance(const blVector2f& other) const noexcept
{
    const float xm = (other.x - x);
    const float ym = (other.y - y);
    return std::sqrt(xm*xm + ym*ym);
}
