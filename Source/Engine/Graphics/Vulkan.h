#pragma once

#include "volk.h"

#ifdef BLUEMETAL_COMPILER_APPLE_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullable-to-nonnull-conversion"
#endif

#define VMA_DEBUG_LOG_FORMAT(...) printf(__VA_ARGS__); putc('\n', stdout)

#include <vk_mem_alloc.h>

#ifdef BLUEMETAL_COMPILER_APPLE_CLANG
#pragma clang diagnostic pop
#endif

#include "Core/MacroUtils.h"

#define VK_CHECK(result)      \
    if (result != VK_SUCCESS) \
        throw std::runtime_error("Vulkan failed: " BL_STRINGIFY(result));
