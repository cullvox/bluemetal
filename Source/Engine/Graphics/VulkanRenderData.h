#pragma once

#include "Vulkan.h"

namespace bl {

struct VulkanRenderData {
    VkCommandBuffer cmd;
    uint32_t currentFrame;
};

} // namespace bl