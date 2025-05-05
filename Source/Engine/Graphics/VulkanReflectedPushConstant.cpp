#include "VulkanReflectedPushConstant.h"

namespace bl
{

VulkanReflectedPushConstant::VulkanReflectedPushConstant(VkShaderStageFlags stages, uint32_t offset, uint32_t size)
    : _range({stages, offset, size}) {}

void VulkanReflectedPushConstant::SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size) {
    _range.stageFlags = stages;
    _range.offset = offset;
    _range.size = size;
}

void VulkanReflectedPushConstant::AddStageFlags(VkShaderStageFlags stages) {
    _range.stageFlags |= stages;
}

bool VulkanReflectedPushConstant::Compare(uint32_t offset, uint32_t size) const {
    return _range.offset == offset && _range.size == size;
}

VkShaderStageFlags VulkanReflectedPushConstant::GetStages() const {
    return _range.stageFlags;
}

VkPushConstantRange VulkanReflectedPushConstant::GetRange() const {
    return _range;
}

} // namespace bl
