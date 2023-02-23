#pragma once

#include "Math/Vector2.hpp"

#include <vector>

namespace bl {

class VideoMode {
public:
    VideoMode() = default;
    VideoMode(Extent2D resolution, unsigned int bitsPerPixel, unsigned int refreshRate);
    ~VideoMode() = default;

    Extent2D resolution;
    unsigned int bitsPerPixel;
    unsigned int refreshRate;
};

};