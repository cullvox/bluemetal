#include "Vector2.h"

namespace bl
{

//===========================
// Vector2i
//===========================

Vector2i::Vector2i(int32_t x, int32_t y)
    : x(x), y(y)
{
}

Vector2i::Vector2i(int32_t xy)
    : x(xy), y(xy)
{
}

Vector2i::Vector2i(const Vector2i& rhs)
    : x(rhs.x), y(rhs.y)
{
}

Vector2i& Vector2i::operator=(const Vector2i& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

Vector2i& Vector2i::operator+(const Vector2i& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2i& Vector2i::operator-(const Vector2i& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2i& Vector2i::operator*(const Vector2i& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2i& Vector2i::operator/(const Vector2i& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

int32_t Vector2i::magnitude()
{
    return (int32_t)std::sqrt(x*x + y*y);
}

int32_t Vector2i::distance(const Vector2i& other)
{
    const int32_t xm = (other.x - x);
    const int32_t ym = (other.y - y);
    return (int32_t)std::sqrt(xm*xm + ym*ym);
}

//===========================
// Vector2f
//===========================

Vector2f::Vector2f(float x, float y)
    : x(x), y(y)
{
}

Vector2f::Vector2f(float xy)
    : x(xy), y(xy)
{
}

Vector2f& Vector2f::operator=(const Vector2f& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

Vector2f& Vector2f::operator+(const Vector2f& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2f& Vector2f::operator-(const Vector2f& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2f& Vector2f::operator*(const Vector2f& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2f& Vector2f::operator/(const Vector2f& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

float Vector2f::magnitude()
{
    return std::sqrt(x*x + y*y);
}

float Vector2f::distance(const Vector2f& other)
{
    const float xm = (other.x - x);
    const float ym = (other.y - y);
    return std::sqrt(xm*xm + ym*ym);
}

//===========================
// Extent2D
//===========================

Extent2D::Extent2D(uint32_t width, uint32_t height)
    : width(width), height(height)
{
}

Extent2D::Extent2D(uint32_t widthHeight)
    : width(widthHeight), height(widthHeight)
{
}

Extent2D::Extent2D(VkExtent2D extent)
    : width(extent.width), height(extent.height)
{
}

Extent2D::operator VkExtent2D()
{
    return VkExtent2D(width, height);
}

//===========================
// Extent2D
//===========================

Rect2D::Rect2D(Vector2i offset, Extent2D extent)
    : offset(offset), extent(extent)
{
}

Rect2D::Rect2D(int32_t offsetX, int32_t offsetY, uint32_t extentX, uint32_t extentY)
    : offset(offsetX, offsetY), extent(extentX, extentY) 
{
}

} // namespace bl