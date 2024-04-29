#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Graphics/Vulkan.h"
#include "Graphics/Device.h"

namespace bl 
{

// VkDescriptorSetLayout objects will have the same underlying pointer value when shared, so they can just be hashed as intptrs
// VkPushConstantRanges can be hash joined.

struct PipelineLayoutInfo 
{
    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VkPushConstantRange> ranges;

    bool operator==(const PipelineLayoutInfo& rhs) const noexcept
    {
        if (setLayouts != rhs.setLayouts) return false;

        for (size_t i = 0; i < rhs.ranges.size(); i++) {
            if (ranges[i].stageFlags != rhs.ranges[i].stageFlags) return false;
            if (ranges[i].offset != rhs.ranges[i].offset) return false;
            if (ranges[i].size != rhs.ranges[i].size) return false;
        }

        return true;
    }
};

struct PipelineLayoutInfoHash
{
    std::size_t operator()(const PipelineLayoutInfo& info) const noexcept
    {
        std::size_t seed = 0;

        for (auto sl : info.setLayouts)
        {
            seed = hash_combine(seed, sl);
        }

        for (const auto& pcr : info.ranges) 
        {
            seed = hash_combine(seed, pcr.stageFlags);
            seed = hash_combine(seed, pcr.offset);
            seed = hash_combine(seed, pcr.size);
        }

        return seed;
    }
};

class PipelineLayoutCache
{
public:
    PipelineLayoutCache(Device* device);
    ~PipelineLayoutCache();

    VkPipelineLayout acquire(std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> ranges);

private:
    Device* _device;
    std::unordered_map<PipelineLayoutInfo, VkPipelineLayout, PipelineLayoutInfoHash> _cache;
};

} // namespace bl