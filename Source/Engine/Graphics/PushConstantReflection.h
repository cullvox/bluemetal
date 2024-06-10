#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "BlockMeta.h"

namespace bl
{

class PushConstantReflection : public BlockMeta
{
public:
    PushConstantReflection() = default;
    PushConstantReflection(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    ~PushConstantReflection() = default;

    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    void AddStageFlags(VkShaderStageFlags stages);
    bool Compare(uint32_t offset, uint32_t size) const;
    VkPushConstantRange GetRange() const;
    VkShaderStageFlags GetStages() const;

private:
    VkPushConstantRange _range;
};

} // namespace bl