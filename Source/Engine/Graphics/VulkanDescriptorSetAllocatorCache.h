#pragma once

#include "VulkanDevice.h"

namespace bl {

struct VulkanDescriptorRatio {
    VulkanDescriptorRatio(VkDescriptorType type, float ratio)
        : type(type), ratio(ratio) {}

    VkDescriptorType type;
    float ratio;

    static std::vector<VulkanDescriptorRatio>& Default()
    {
        static auto ratios = std::vector<VulkanDescriptorRatio>{
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
public:

    /// @brief Descriptor Set Cache Constructor
    /// @param[in] device The device used to create descriptor sets.
    VulkanDescriptorSetAllocatorCache(
        const VulkanDevice* device, 
        uint32_t maxSets, 
        std::span<VulkanDescriptorRatio> ratios);

    /// @brief Destructor
    ~VulkanDescriptorSetAllocatorCache();

    /// @brief Allocates/retrieves a descriptor set created with this layout.
    VkDescriptorSet Allocate(VkDescriptorSetLayout layout);

    /// @brief Frees the descriptor set back into the cache.
    void Free(VkDescriptorSetLayout layout, VkDescriptorSet set);

    /// @brief Completely resets every descriptor set in every pool, use before destroying resources.  */
    void ResetPools();

private:
    
    /// @brief Grabs us a new pool allocating as required.
    VkDescriptorPool GrabPool();

    /// @brief Creates a new pool after the previous one was used up.
    VkDescriptorPool CreatePool(uint32_t setCount); 

    const VulkanDevice* _device;
    std::unordered_map<VkDescriptorSetLayout, std::unordered_set<VkDescriptorSet>> _freeSets;
    uint32_t _setsPerPool;
    std::vector<VulkanDescriptorRatio> _ratios;
    std::vector<VkDescriptorPool> _usedPools;
    std::vector<VkDescriptorPool> _freePools;
};

} // namespace bl