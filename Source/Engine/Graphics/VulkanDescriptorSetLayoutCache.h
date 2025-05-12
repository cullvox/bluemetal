#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl {

class VulkanDevice;

/// @brief Data used by the descriptor set layout cache.
struct VulkanDescriptorLayoutCacheData {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bool operator==(const VulkanDescriptorLayoutCacheData& rhs) const;
};

/// @brief Helper to hash descriptor set layout data.
struct VulkanDescriptorLayoutCacheHasher {
    size_t operator()(const VulkanDescriptorLayoutCacheData& data) const noexcept;
};

/// @brief Caches descriptor set layouts optimizing descriptor set creation speeds and memory usage.
class VulkanDescriptorSetLayoutCache {
public:

    /// @brief Layout Cache Constructor
    /// @param device The Vulkan device used to create descriptor set layouts.
    VulkanDescriptorSetLayoutCache(const VulkanDevice* device);

    /// @brief Destructor
    ~VulkanDescriptorSetLayoutCache();

    /// @brief Creates or retrieves a descriptor set layout from it's bindings.
    VkDescriptorSetLayout Acquire(std::span<VkDescriptorSetLayoutBinding> bindings);

private:
    const VulkanDevice* _device;
    std::unordered_map<VulkanDescriptorLayoutCacheData, VkDescriptorSetLayout, VulkanDescriptorLayoutCacheHasher> _cache; /** @brief Hashmap caches all the descriptor set layouts. */
};

} // namespace bl
