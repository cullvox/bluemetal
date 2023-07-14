#pragma once

#include "Math/Vector2.h"

struct blVideoMode
{
    blVideoMode() = default;
    blVideoMode(blExtent2D extent, uint8_t bitsPerPixel, uint16_t refreshRate)
        : extent(extent)
        , bitsPerPixel(bitsPerPixel)
        , refreshRate(refreshRate)
    {
    }

    blExtent2D extent;      // size of the display mode in pixels
    uint8_t bitsPerPixel;   // amount of bits for each pixel
    uint16_t refreshRate;   // refresh rate of the monitor in hz
};