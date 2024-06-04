#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "BlockMeta.h"

namespace bl
{

class PushConstantMeta : public BlockMeta
{
public:
    PushConstantMeta() = default;
    PushConstantMeta(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    ~PushConstantMeta() = default;

    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    void AddStageFlags(VkShaderStageFlags stages);
    bool Compare(uint32_t offset, uint32_t size) const;
    VkPushConstantRange GetRange() const;

private:
    VkPushConstantRange _range;
};

} // namespace bl