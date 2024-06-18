#pragma once

#include "Graphics/VulkanRenderData.h"
#include "Vulkan.h"
#include "VulkanConfig.h"

namespace bl {



class VulkanDescriptorSet {
public:
    VulkanDescriptorSet();
    ~VulkanDescriptorSet();

private:
    VkDescriptorSet _set;
};

class VulkanPerFrameDescriptorSet {
public:
    VulkanDescriptorSet();
    ~VulkanDescriptorSet();

    void Bind(VulkanRenderData& rd);

private:
    std::array<VkDescriptorSet, VulkanConfig::numFramesInFlight> _set;
};

} // namespace bl

