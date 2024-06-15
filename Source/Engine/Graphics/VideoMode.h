#pragma once

#include "Vulkan.h"

namespace bl {

struct VideoMode 
{
    VideoMode(VkRect2D rect, int refreshRate)
        : rect(rect), refreshRate(refreshRate) {}

    VkRect2D rect; /** @brief Size of the display mode in pixels. */
    int refreshRate; /** @brief Refresh rate of the monitor in hz. */
};

} // namespace bl
