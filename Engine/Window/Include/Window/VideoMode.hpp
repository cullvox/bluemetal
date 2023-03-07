#pragma once

#include "Math/Vector2.hpp"

namespace bl {

/// @brief Video modes for creating windows.
struct DisplayMode {
public:

    /// @brief Default constructor
    DisplayMode() = default;

    /// @brief Constructs a user defined video mode, may be invalid.
    /// @param extent The size of the display mode in pixels.
    /// @param bitsPerPixel Amount of bits for each pixel.
    /// @param refreshRate Refresh rate in hz of the monitor.
    DisplayMode(Extent2D extent, uint8_t bitsPerPixel, uint16_t refreshRate)
        : extent(extent), bitsPerPixel(bitsPerPixel), refreshRate(refreshRate)
    { }

    /// @brief Default destructor
    ~DisplayMode() = default;


    /// @brief The size of the display mode in pixels.
    Extent2D extent;

    /// @brief Amount of bits for each pixel.
    uint8_t bitsPerPixel;

    /// @brief Refresh rate in hz of the monitor.
    uint16_t refreshRate;

};

} // namespace bl