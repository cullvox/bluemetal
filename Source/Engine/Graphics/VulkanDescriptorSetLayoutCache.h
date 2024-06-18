#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl {

class VulkanDevice;

/** @brief Data used by the descriptor set layout cache */
struct VulkanDescriptorLayoutCacheData {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bool operator==(const VulkanDescriptorLayoutCacheData& rhs) const;
};

/** @brief Hashes descriptor set layout data. */
struct VulkanDescriptorLayoutCacheHasher {
    size_t operator()(const VulkanDescriptorLayoutCacheData& data) const noexcept;
};

/** @brief Caches descriptor set layouts optimizing descriptor set creation speeds and memory usage. */
class VulkanDescriptorSetLayoutCache {
public:
    VulkanDescriptorSetLayoutCache(VulkanDevice* device); /** @brief Constructor */
    ~VulkanDescriptorSetLayoutCache(); /** @brief Destructor */

    VkDescriptorSetLayout Acquire(const std::vector<VkDescriptorSetLayoutBinding>& bindings); /** @brief Creates or retrieves a descriptor set layout from it's bindings. */

private:
    VulkanDevice* _device;
    std::unordered_map<VulkanDescriptorLayoutCacheData, VkDescriptorSetLayout, VulkanDescriptorLayoutCacheHasher> _cache; /** @brief Hashmap caches all the descriptor set layouts. */
};

} // namespace bl
