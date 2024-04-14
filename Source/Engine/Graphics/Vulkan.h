#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "volk.h"
#include "vk_mem_alloc.h"

#include "Core/MacroUtils.h"

#define VK_CHECK(result) \
    if (result != VK_SUCCESS) throw std::runtime_error("Vulkan failed: " BL_STRINGIFY(result));


