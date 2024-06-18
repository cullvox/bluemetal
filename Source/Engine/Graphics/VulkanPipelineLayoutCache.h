#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Graphics/Vulkan.h"

namespace bl 
{

class VulkanDevice;

struct VulkanPipelineLayoutCacheData {
    std::vector<VkDescriptorSetLayout> layouts;
    std::vector<VkPushConstantRange> ranges;
    
    bool operator==(const VulkanPipelineLayoutCacheData& rhs) const noexcept;
};

struct VulkanPipelineLayoutCacheHasher
{
    std::size_t operator()(const VulkanPipelineLayoutCacheData& data) const noexcept;
};

class VulkanPipelineLayoutCache
{
public:
    VulkanPipelineLayoutCache(VulkanDevice* device);
    ~VulkanPipelineLayoutCache();

    VkPipelineLayout Acquire(const std::span<VkDescriptorSetLayout> layouts, const std::span<VkPushConstantRange> ranges);

private:
    VulkanDevice* _device;
    std::unordered_map<VulkanPipelineLayoutCacheData, VkPipelineLayout, VulkanPipelineLayoutCacheHasher> _cache;
};

} // namespace bl