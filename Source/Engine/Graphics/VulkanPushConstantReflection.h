#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "VulkanBlockReflection.h"

namespace bl {

class VulkanPushConstantReflection : public VulkanBlockReflection {
public:
    VulkanPushConstantReflection() = default;
    VulkanPushConstantReflection(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    ~VulkanPushConstantReflection() = default;

    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    void AddStageFlags(VkShaderStageFlags stages);
    bool Compare(uint32_t offset, uint32_t size) const;
    VkPushConstantRange GetRange() const;
    VkShaderStageFlags GetStages() const;

private:
    VkPushConstantRange _range;
};

} // namespace bl