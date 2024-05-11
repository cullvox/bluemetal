#pragma once

#include "Device.h"

namespace bl
{

struct DescriptorRatio
{
    VkDescriptorType type;
    float ratio;

    static std::vector<DescriptorRatio> Default()
    {
        return std::vector<DescriptorRatio>{
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
    }
};

class DescriptorSetAllocator
{
public:
    DescriptorSetAllocator(Device* device, uint32_t maxSets, std::span<DescriptorRatio> ratios);
    ~DescriptorSetAllocator();

    VkDescriptorSet Allocate(VkDescriptorSetLayout layout);
    void ResetPools();

private:
    VkDescriptorPool GrabPool();
    VkDescriptorPool CreatePool(uint32_t setCount);

    Device* _device;
    uint32_t _setsPerPool;
    std::vector<DescriptorRatio> _ratios;
    std::vector<VkDescriptorPool> _usedPools;
    std::vector<VkDescriptorPool> _freePools;
};

} // namespace bl