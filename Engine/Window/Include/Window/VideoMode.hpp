#pragma once

#include "Math/Vector2.hpp"

namespace bl {

/// @brief Video modes for creating windows.
struct VideoMode {
public:

    /// @brief Default constructor
    VideoMode() = default;

    /// @brief Constructs a user defined video mode, may be invalid.
    /// @param resolution Position of the window along with its extent.
    /// @param bitsPerPixel Amount of bits for each pixel.
    /// @param refreshRate Refresh rate in hz of the monitor.
    VideoMode(Rect2D rect, uint8_t bitsPerPixel, uint16_t refreshRate);

    /// @brief Default destructor
    ~VideoMode() = default;


    /// @brief Position of the window along with its extent.
    Rect2D rect;

    /// @brief Amount of bits for each pixel.
    uint8_t bitsPerPixel;

    /// @brief Refresh rate in hz of the monitor.
    uint16_t refreshRate;

};

} // namespace bl