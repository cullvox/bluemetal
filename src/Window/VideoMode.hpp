#pragma once

#include "Math/Vector2.hpp"

#include <vector>
#include <optional>
#include <functional>

namespace bl {

class Screen;

class VideoMode {
public:
    VideoMode() = default;
    VideoMode(Extent2D resolution, unsigned int bitsPerPixel, unsigned int refreshRate);
    ~VideoMode() = default;

    Extent2D resolution;
    unsigned int bitsPerPixel;
    unsigned int refreshRate;
    std::optional<std::reference_wrapper<Screen>> screen;
};

};