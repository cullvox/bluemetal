#pragma once

#include "Math/Vector2.hpp"

struct blDisplayMode  // Video modes for creating windows.
{
    blExtent2D extent; // The size of the display mode in pixels.
    uint8_t bitsPerPixel; // Amount of bits for each pixel.
    uint16_t refreshRate; // Refresh rate in hz of the monitor.
};