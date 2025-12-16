#pragma once

#include "Vulkan.h"

namespace bl {

class Renderer;

struct VulkanRenderData {
    Renderer* renderer;
    VkCommandBuffer cmd;
    uint32_t currentFrame;
    uint32_t imageIndex;
    VkDescriptorSet globalSet;
};

} // namespace bl