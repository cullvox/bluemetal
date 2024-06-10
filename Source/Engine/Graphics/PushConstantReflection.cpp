#include "PushConstantReflection.h"

namespace bl
{

PushConstantReflection::PushConstantReflection(VkShaderStageFlags stages, uint32_t offset, uint32_t size)
    : _range({stages, offset, size})
{
}

void PushConstantReflection::SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size)
{
    _range.stageFlags = stages;
    _range.offset = offset;
    _range.size = size;
}

void PushConstantReflection::AddStageFlags(VkShaderStageFlags stages)
{
    _range.stageFlags |= stages;
}

bool PushConstantReflection::Compare(uint32_t offset, uint32_t size) const
{
    return _range.offset == offset && _range.size == size;
}

VkShaderStageFlags PushConstantReflection::GetStages() const
{
    return _range.stageFlags;
}

VkPushConstantRange PushConstantReflection::GetRange() const
{
    return _range;
}

} // namespace bl
