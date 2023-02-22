#pragma once

namespace bl
{

class Vector2i
{
public:
    Vector2i() noexcept;
    Vector2i(long x, long z) noexcept;
    Vector2i(long xy) noexcept;
    Vector2i(const Vector2i& rhs) noexcept;

    Vector2i& operator=(const Vector2i& rhs) noexcept;
    Vector2i& operator+(const Vector2i& rhs) noexcept;
    Vector2i& operator-(const Vector2i& rhs) noexcept;
    Vector2i& operator*(const Vector2i& rhs) noexcept;
    Vector2i& operator/(const Vector2i& rhs) noexcept;

    int         magnitude() const noexcept;
    int         distance(const Vector2i& other) const noexcept;
    Vector2i    cross(const Vector2i& other) const noexcept;

    long x;
    long y;
};

class Vector2f
{
public:
    Vector2f() noexcept;
    Vector2f(float x, float z) noexcept;
    Vector2f(float xy) noexcept;
    Vector2f(const Vector2i& rhs) noexcept;

    Vector2f& operator=(const Vector2f& rhs) const noexcept;
    Vector2f& operator+(const Vector2f& rhs) const noexcept;
    Vector2f& operator-(const Vector2f& rhs) const noexcept;
    Vector2f& operator*(const Vector2f& rhs) const noexcept;
    Vector2f& operator/(const Vector2f& rhs) const noexcept;

    float magnitude() const noexcept;
    float distance(const Vector2i& other) const noexcept;
    Vector2i cross(const Vector2i& other) const noexcept;

    float x;
    float y;
};

class Extent2D
{
public:
    Extent2D();
    Extent2D(unsigned long width, unsigned long height);
    Extent2D(unsigned long wh);

    unsigned long width;
    unsigned long height;
};

} /* namespace bl*/