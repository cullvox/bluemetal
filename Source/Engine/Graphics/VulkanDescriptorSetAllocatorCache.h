#pragma once

#include "VulkanDevice.h"
#include "VulkanDescriptorSet.h"

namespace bl {

struct VulkanDescriptorRatio {
    VulkanDescriptorRatio(VkDescriptorType type, float ratio)
        : type(type)
        , ratio(ratio)
    {
    }

    VkDescriptorType type;
    float ratio;

    static std::vector<VulkanDescriptorRatio>& Default()
    {
        static auto ratios = std::vector<VulkanDescriptorRatio> {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
        };

        return ratios;
    }
};

/// @brief Tracks descriptor sets that aren't being used anymore.
/// Will allocate more from a growable descriptor set pool as needed.
class VulkanDescriptorSetAllocatorCache {
    VkDescriptorPool GrabPool(); /** @brief Grabs us a new pool allocating as required. */
    VkDescriptorPool CreatePool(uint32_t setCount); /** @brief Creates a new pool after the previous one was used up. */

    const VulkanDevice* _device;
    std::unordered_map<VkDescriptorSetLayout, std::unordered_set<VkDescriptorSet>> _freeSets;
    uint32_t _setsPerPool;
    std::vector<VulkanDescriptorRatio> _ratios;
    std::vector<VkDescriptorPool> _usedPools;
    std::vector<VkDescriptorPool> _freePools;

protected:
    friend class VulkanDescriptorSet;
    VkDescriptorSet AllocateRaw(VkDescriptorSetLayout layout);
    void FreeRaw(VkDescriptorSetLayout layout, VkDescriptorSet set);

    VulkanDescriptorSetAllocatorCache();
public:
    VulkanDescriptorSetAllocatorCache(const VulkanDevice* device, uint32_t maxSets, std::span<VulkanDescriptorRatio> ratios);
    VulkanDescriptorSetAllocatorCache(const VulkanDescriptorSetAllocatorCache&) = delete;
    VulkanDescriptorSetAllocatorCache(VulkanDescriptorSetAllocatorCache&&) = default;
    ~VulkanDescriptorSetAllocatorCache();

    VulkanDescriptorSetAllocatorCache& operator=(const VulkanDescriptorSetAllocatorCache&) = delete;
    VulkanDescriptorSetAllocatorCache& operator=(VulkanDescriptorSetAllocatorCache&&) = default;

    VulkanDescriptorSet Allocate(VulkanDescriptorSetLayout& layout); /** @brief Allocates/retrieves a descriptor set created with this layout. */
    void ResetPools(); /** @brief Completely resets every descriptor set in every pool, use before destroying resources. */
};

} // namespace bl