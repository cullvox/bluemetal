#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl
{

class Device;

/** @brief Data used by the descriptor set layout cache */
struct DescriptorLayoutCacheData
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bool operator==(const DescriptorLayoutCacheData& rhs) const;
};

/** @brief Hashes descriptor set layout data. */
struct DescriptorLayoutCacheHasher
{
    size_t operator()(const DescriptorLayoutCacheData& data) const noexcept;
};

/** @brief Caches descriptor set layouts optimizing descriptor set creation speeds and memory usage. */
class DescriptorSetLayoutCache
{
public:
    DescriptorSetLayoutCache(Device* device); /** @brief Constructor */
    ~DescriptorSetLayoutCache(); /** @brief Destructor */

    VkDescriptorSetLayout Acquire(const std::vector<VkDescriptorSetLayoutBinding>& bindings); /** @brief Creates or retrieves a descriptor set layout from it's bindings. */

private:
    Device* _device;
    std::unordered_map<DescriptorLayoutCacheData, VkDescriptorSetLayout, DescriptorLayoutCacheHasher> _cache; /** @brief Hashmap caches all the descriptor set layouts. */
};

} // namespace bl
