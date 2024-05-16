#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "BlockReflection.h"

namespace bl
{

class PushConstantRangeBuilder
{
public:
    PushConstantRangeBuilder();
    ~PushConstantRangeBuilder();
    
    void AddStageFlags(VkShaderStageFlags flags);
    BlockMember& AccessOrInsertMember(std::string name);
    std::vector<BlockMember> GetMembers() const;
    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);
    void AddStageFlags(VkShaderStageFlags stages);
    bool Compare(uint32_t offset, uint32_t size);

    BlockMember& operator[](const std::string& name); // Shorthanded for AccessOrInsertMember().

private:
    std::unordered_set<std::string, BlockMember> _members;
    VkPushConstantRange _range;
}

}