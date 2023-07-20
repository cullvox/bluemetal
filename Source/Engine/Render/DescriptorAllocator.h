#pragma once

#include "Device.h"

class blDescriptorAllocator
{
public:
    explicit blDescriptorAllocator(std::shared_ptr<blDevice> device);
    ~blDescriptorAllocator();

    void resetPools();
    VkDescriptorSet allocate(VkDescriptorSetLayout layout);

    struct PoolSizes
    {
        std::vector<std::pair<VkDescriptorType, float>> sizes =
        {    
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
    };
private:
    VkDescriptorPool grabPool();

    std::shared_ptr<blDevice>       _device;
    VkDescriptorPool                _currentPool;
    PoolSizes                       _descriptorSizes;
    std::vector<VkDescriptorPool>   _usedPools;
    std::vector<VkDescriptorPool>   _freePools;
};