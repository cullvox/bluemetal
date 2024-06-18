#include "VulkanPushConstantReflection.h"

namespace bl
{

VulkanPushConstantReflection::VulkanPushConstantReflection(VkShaderStageFlags stages, uint32_t offset, uint32_t size)
    : _range({stages, offset, size}) {}

void VulkanPushConstantReflection::SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size) {
    _range.stageFlags = stages;
    _range.offset = offset;
    _range.size = size;
}

void VulkanPushConstantReflection::AddStageFlags(VkShaderStageFlags stages) {
    _range.stageFlags |= stages;
}

bool VulkanPushConstantReflection::Compare(uint32_t offset, uint32_t size) const {
    return _range.offset == offset && _range.size == size;
}

VkShaderStageFlags VulkanPushConstantReflection::GetStages() const {
    return _range.stageFlags;
}

VkPushConstantRange VulkanPushConstantReflection::GetRange() const {
    return _range;
}

} // namespace bl
