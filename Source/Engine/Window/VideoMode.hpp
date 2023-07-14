#pragma once

#include "Math/Vector2.hpp"

struct blDisplayMode
{
    blExtent2D extent;      // size of the display mode in pixels
    uint8_t bitsPerPixel;   // amount of bits for each pixel
    uint16_t refreshRate;   // refresh rate of the monitor in hz
};