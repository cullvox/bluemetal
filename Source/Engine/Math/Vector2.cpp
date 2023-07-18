#include "Vector2.h"

//===========================
// blVector2i
//===========================

blVector2i::blVector2i(int32_t x, int32_t y)
    : x(x), y(y)
{
}

blVector2i::blVector2i(int32_t xy)
    : x(xy), y(xy)
{
}

blVector2i::blVector2i(const blVector2i& rhs)
    : x(rhs.x), y(rhs.y)
{
}

blVector2i& blVector2i::operator=(const blVector2i& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

blVector2i& blVector2i::operator+(const blVector2i& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

blVector2i& blVector2i::operator-(const blVector2i& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

blVector2i& blVector2i::operator*(const blVector2i& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

blVector2i& blVector2i::operator/(const blVector2i& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

int32_t blVector2i::magnitude()
{
    return (int32_t)std::sqrt(x*x + y*y);
}

int32_t blVector2i::distance(const blVector2i& other)
{
    const int32_t xm = (other.x - x);
    const int32_t ym = (other.y - y);
    return (int32_t)std::sqrt(xm*xm + ym*ym);
}

//===========================
// blVector2f
//===========================

blVector2f::blVector2f(float x, float y)
    : x(x), y(y)
{
}

blVector2f::blVector2f(float xy)
    : x(xy), y(xy)
{
}

blVector2f& blVector2f::operator=(const blVector2f& rhs)
{
    x = rhs.x;
    y = rhs.y;
    return *this;
}

blVector2f& blVector2f::operator+(const blVector2f& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

blVector2f& blVector2f::operator-(const blVector2f& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

blVector2f& blVector2f::operator*(const blVector2f& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

blVector2f& blVector2f::operator/(const blVector2f& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

float blVector2f::magnitude()
{
    return std::sqrt(x*x + y*y);
}

float blVector2f::distance(const blVector2f& other)
{
    const float xm = (other.x - x);
    const float ym = (other.y - y);
    return std::sqrt(xm*xm + ym*ym);
}

//===========================
// blExtent2D
//===========================

blExtent2D::blExtent2D(uint32_t width, uint32_t height)
    : width(width), height(height)
{
}

blExtent2D::blExtent2D(uint32_t widthHeight)
    : width(widthHeight), height(widthHeight)
{
}

blExtent2D::blExtent2D(VkExtent2D extent)
    : width(extent.width), height(extent.height)
{
}

blExtent2D::operator VkExtent2D()
{
    return VkExtent2D(width, height);
}

//===========================
// blExtent2D
//===========================

blRect2D::blRect2D(blVector2i offset, blExtent2D extent)
    : offset(offset), extent(extent)
{
}

blRect2D::blRect2D(int32_t offsetX, int32_t offsetY, uint32_t extentX, uint32_t extentY)
    : offset(offsetX, offsetY), extent(extentX, extentY) 
{
}

