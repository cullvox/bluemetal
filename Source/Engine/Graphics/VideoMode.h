#pragma once

#include "Vulkan.h"

namespace bl {

struct VideoMode 
{
    VkRect2D rect; /** @brief Size of the display mode in pixels. */
    int refreshRate; /** @brief Refresh rate of the monitor in hz. */
};

} // namespace bl
