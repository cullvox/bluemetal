#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "VulkanReflectedBlock.h"

namespace bl {

class VulkanReflectedPushConstant : public VulkanReflectedBlock {
public:
    VulkanReflectedPushConstant() = default;
    VulkanReflectedPushConstant(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    ~VulkanReflectedPushConstant() = default;

    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    void AddStageFlags(VkShaderStageFlags stages);
    bool Compare(uint32_t offset, uint32_t size) const;
    VkPushConstantRange GetRange() const;
    VkShaderStageFlags GetStages() const;

private:
    VkPushConstantRange _range;
};

} // namespace bl