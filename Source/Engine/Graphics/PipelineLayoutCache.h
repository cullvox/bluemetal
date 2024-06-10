#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Graphics/Vulkan.h"

namespace bl 
{

class Device;

struct PipelineLayoutCacheData 
{
    std::vector<VkDescriptorSetLayout> layouts;
    std::vector<VkPushConstantRange> ranges;
    
    bool operator==(const PipelineLayoutCacheData& rhs) const noexcept;
};

struct PipelineLayoutCacheHasher
{
    std::size_t operator()(const PipelineLayoutCacheData& data) const noexcept;
};

class PipelineLayoutCache
{
public:
    PipelineLayoutCache(Device* device);
    ~PipelineLayoutCache();

    VkPipelineLayout Acquire(const std::span<VkDescriptorSetLayout> layouts, const std::span<VkPushConstantRange> ranges);

private:
    Device* _device;
    std::unordered_map<PipelineLayoutCacheData, VkPipelineLayout, PipelineLayoutCacheHasher> _cache;
};

} // namespace bl