#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl {

class VulkanDevice;

/// @brief Data stored in the layout cache per pipeline layout.
struct VulkanPipelineLayoutCacheData {
    std::vector<VkDescriptorSetLayout> layouts;
    std::vector<VkPushConstantRange> ranges;
    bool operator==(const VulkanPipelineLayoutCacheData& rhs) const noexcept;
};

/// @brief Hash helper to hash stored pipeline layout cache data.
struct VulkanPipelineLayoutCacheHasher {
    std::size_t operator()(const VulkanPipelineLayoutCacheData& data) const noexcept;
};

/// @brief Caches Vulkan pipeline layout objects to be reused if similar.
class VulkanPipelineLayoutCache {
public:

    /// @brief Cache Constructor
    /// @param[in] device Device used to create pipeline layouts with.
    VulkanPipelineLayoutCache(const VulkanDevice* device);
    
    /// @brief Destructor
    ~VulkanPipelineLayoutCache();

    /// @brief Creates a new pipeline layout or retrieves an already cached layout.
    /// @param[in] layouts Descriptor set layouts used in this pipeline layout.
    /// @param[in] ranges Push constants used in this pipeline layout.
    VkPipelineLayout Acquire(const std::span<VkDescriptorSetLayout> layouts, const std::span<VkPushConstantRange> ranges);

private:
    const VulkanDevice* _device;
    std::unordered_map<VulkanPipelineLayoutCacheData, VkPipelineLayout, VulkanPipelineLayoutCacheHasher> _cache;
};

} // namespace bl