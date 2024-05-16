#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "BlockReflection.h"

namespace bl
{

class PushConstantRangeBuilder : public BlockMembersBuilder
{
public:
    PushConstantRangeBuilder();
    ~PushConstantRangeBuilder();
    
    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    void AddStageFlags(VkShaderStageFlags stages);
    bool Compare(uint32_t offset, uint32_t size);
    VkPushConstantRange GetRange();

private:
    VkPushConstantRange _range;
};

}