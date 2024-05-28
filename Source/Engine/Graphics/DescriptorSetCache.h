#pragma once

#include "Precompiled.h"
#include "Vulkan.h"

namespace bl
{

class Device;

struct DescriptorRatio
{
    VkDescriptorType type;
    float ratio;

    static std::vector<DescriptorRatio>& Default()
    {
        static auto ratios = std::vector<DescriptorRatio>{
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

/** @brief Tracks descriptor sets that aren't being used anymore.
 *  Will allocate more from a growable descriptor set pool as needed.
 */
class DescriptorSetCache
{
public:
    DescriptorSetCache(Device* device, uint32_t maxSets, std::span<DescriptorRatio> ratios); /** @brief Constructor */
    ~DescriptorSetCache(); /** @brief Destructor */

    VkDescriptorSet Allocate(VkDescriptorSetLayout layout); /** @brief Allocates/retrieves a descriptor set created with this layout. */
    void Free(VkDescriptorSetLayout layout, VkDescriptorSet set); /** @brief Frees the descriptor set back into the cache. */

private:
    VkDescriptorPool GrabPool(); /** Grabs us a new pool allocating as required. */
    VkDescriptorPool CreatePool(uint32_t setCount); /** @brief Creates a new pool after the previous one was used up. */
    void ResetPools(); /** @brief Completely resets every descriptor set in every pool, not very useful right now. */

    Device* _device;
    std::unordered_map<VkDescriptorSetLayout, std::unordered_set<VkDescriptorSet>> _freeSets;
    uint32_t _setsPerPool;
    std::vector<DescriptorRatio> _ratios;
    std::vector<VkDescriptorPool> _usedPools;
    std::vector<VkDescriptorPool> _freePools;
};

} // namespace bl