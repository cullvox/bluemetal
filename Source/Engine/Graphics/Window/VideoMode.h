#pragma once

#include "Math/Vector2.h"

namespace bl {

struct VideoMode {
    Extent2D   extent; // Size of the display mode in pixels.
    uint8_t    bitsPerPixel; // Amount of bits for each pixel.
    uint16_t   refreshRate; // Refresh rate of the monitor in hz.
};

} // namespace bl
