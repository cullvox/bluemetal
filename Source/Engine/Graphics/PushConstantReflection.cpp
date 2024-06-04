#include "PushConstantReflection.h"

namespace bl
{

PushConstantMeta::PushConstantMeta(VkShaderStageFlags stages, uint32_t offset, uint32_t size)
    : _range({stages, offset, size})
{
}

void PushConstantMeta::SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size)
{
    _range.stageFlags = stages;
    _range.offset = offset;
    _range.size = size;
}

void PushConstantMeta::AddStageFlags(VkShaderStageFlags stages)
{
    _range.stageFlags |= stages;
}

bool PushConstantMeta::Compare(uint32_t offset, uint32_t size) const
{
    return _range.offset == offset && _range.size == size;
}

VkPushConstantRange PushConstantMeta::GetRange() const
{
    return _range;
}

} // namespace bl
