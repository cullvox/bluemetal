#pragma once

#include "VulkanDescriptorSetLayout.h"

namespace bl {

class VulkanDevice;

/** @brief Data used by the descriptor set layout cache. */
struct VulkanDescriptorLayoutCacheData {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bool operator==(const VulkanDescriptorLayoutCacheData& rhs) const;
};

/** @brief Helper to hash descriptor set layout data. */
struct VulkanDescriptorLayoutCacheHasher {
    size_t operator()(const VulkanDescriptorLayoutCacheData& data) const noexcept;
};

/** @brief Caches descriptor set layouts optimizing descriptor set creation speeds and memory usage. */
class VulkanDescriptorSetLayoutCache {
    const VulkanDevice* _device;
    std::unordered_map<VulkanDescriptorLayoutCacheData, VkDescriptorSetLayout, VulkanDescriptorLayoutCacheHasher> _cache; /** @brief Hashmap caches all the descriptor set layouts. */

protected:
    friend class VulkanDescriptorSetLayout;
    VkDescriptorSetLayout AcquireRaw(std::span<VkDescriptorSetLayoutBinding> bindings);
    void FreeRaw();

public:
    VulkanDescriptorSetLayoutCache(const VulkanDevice* device);
    ~VulkanDescriptorSetLayoutCache();
};

} // namespace bl
