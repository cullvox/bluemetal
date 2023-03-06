#pragma once

#include <cstdint>

namespace bl
{

class Vector2i
{
public:
    Vector2i() noexcept;
    Vector2i(int32_t x, int32_t z) noexcept;
    Vector2i(int32_t xy) noexcept;
    Vector2i(const Vector2i& rhs) noexcept;

    Vector2i& operator=(const Vector2i& rhs) noexcept;
    Vector2i& operator+(const Vector2i& rhs) noexcept;
    Vector2i& operator-(const Vector2i& rhs) noexcept;
    Vector2i& operator*(const Vector2i& rhs) noexcept;
    Vector2i& operator/(const Vector2i& rhs) noexcept;

    int32_t magnitude() const noexcept;
    int32_t distance(const Vector2i& other) const noexcept;

    int32_t x;
    int32_t y;
};

class Vector2f
{
public:
    Vector2f() noexcept;
    Vector2f(float x, float z) noexcept;
    Vector2f(float xy) noexcept;

    Vector2f& operator=(const Vector2f& rhs) noexcept;
    Vector2f& operator+(const Vector2f& rhs) noexcept;
    Vector2f& operator-(const Vector2f& rhs) noexcept;
    Vector2f& operator*(const Vector2f& rhs) noexcept;
    Vector2f& operator/(const Vector2f& rhs) noexcept;

    float magnitude() const noexcept;
    float distance(const Vector2f& other) const noexcept;

    float x;
    float y;
};

class Extent2D
{
public:
    Extent2D();
    Extent2D(uint32_t width, uint32_t height);
    Extent2D(uint32_t widthHeight);

    uint32_t width;
    uint32_t height;
};

struct Rect2D
{
public:
    /// @brief Default constructor
    Rect2D() = default;

    /// @brief Constructs with the offset and extent filled
    /// @param offset Where the rect starts
    /// @param extent How far the rect goes
    Rect2D(Vector2i offset, Extent2D extent)
        : offset(offset), extent(extent) { }

    /// @brief Constructs an expanded version of the rect
    /// @param offsetX offset.x
    /// @param offsetY offset.y
    /// @param extentX extent.x
    /// @param extentY extent.y 
    Rect2D(int32_t offsetX, int32_t offsetY, uint32_t extentX, uint32_t extentY)
        : offset(Vector2i{offsetX, offsetY}), extent(Extent2D{extentX, extentY}) { }

    /// @brief Where the rect starts
    Vector2i offset;

    /// @brief How far the rect goes
    Extent2D extent;
};

} /* namespace bl*/