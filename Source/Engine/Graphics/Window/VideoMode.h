#pragma once

#include "Precompiled.h"

namespace bl {

struct VideoMode 
{
    glm::ivec2 extent; /** @brief Size of the display mode in pixels. */
    uint8_t bitsPerPixel; /** @brief Amount of bits for each pixel. */
    uint16_t refreshRate; /** @brief Refresh rate of the monitor in hz. */
};

} // namespace bl
