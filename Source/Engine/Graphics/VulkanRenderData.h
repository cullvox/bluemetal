#pragma once

#include "Vulkan.h"
#include "UniformData.h"

namespace bl {

class Renderer;
class MaterialInstance;

class VulkanRenderData {



public:

    Renderer* renderer;
    VkCommandBuffer cmd;
    uint32_t currentFrame;
    uint32_t imageIndex;
    VkDescriptorSet globalSet;
};

} // namespace bl