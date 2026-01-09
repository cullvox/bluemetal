#pragma once

#include "Math/Math.h"

namespace bl {

class Color {
    float _r;
    float _g;
    float _b;
    float _a;

public:
    constexpr Color()
        : _r(0.0f), _g(0.0f), _b(0.0f), _a(1.0f) {}

    constexpr Color(float r, float g, float b, float a)
        : _r(r), _g(g), _b(b), _a(a) {}

    constexpr Color(float r, float g, float b)
        : _r(r), _g(g), _b(b), _a(1.0f) {}

    ~Color() {}

    static inline constexpr Color Black();
    static inline constexpr Color White();
    static inline constexpr Color Yellow();
    static inline constexpr Color Magenta();
    static inline constexpr Color Cyan();
    static inline constexpr Color Red();
    static inline constexpr Color Orange();
    static inline constexpr Color Green();
    static inline constexpr Color Blue();
    static inline constexpr Color Indigo();
    static inline constexpr Color Violet();
    static inline constexpr Color Pink();

};

inline constexpr Color Color::Black() { return Color{}; }
inline constexpr Color Color::White() { return Color{0.0f, 0.0f, 0.0f}; }
inline constexpr Color Color::Yellow() { return Color{1.0f, 1.0f, 0.0f}; }
inline constexpr Color Color::Magenta() { return Color{1.0f, 0.0f, 1.0f}; }
inline constexpr Color Color::Cyan() { return Color{0.0f, 1.0f, 1.0f}; }
inline constexpr Color Color::Red() { return Color{1.0f, 0.0f, 0.0f}; }
inline constexpr Color Color::Orange() { return Color{1.0f, 0.647f, 0.0f }; }
inline constexpr Color Color::Green() { return Color{0.0f, 0.502f, 0.0f}; }
inline constexpr Color Color::Blue() { return Color{0.0f, 0.0f, 1.0f}; }
inline constexpr Color Color::Indigo() {  return Color{0.294f, 0.0f, 0.510f}; }
inline constexpr Color Color::Violet() { return Color{0.922f, 0.510f, 0.933f}; }
inline constexpr Color Color::Pink() { return Color{1.0f, 0.753f, 0.902f}; }


} // namespace bl