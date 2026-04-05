#pragma once


#include "Core/MacroUtils.h"

#define VK_CHECK(result)      \
    if (result != VK_SUCCESS) \
        throw std::runtime_error("Vulkan failed: " BL_STRINGIFY(result));
