#pragma once

#include "Device.h"

namespace bl
{

class DescriptorAllocator
{
public:
    DescriptorAllocator(std::shared_ptr<Device> device);
    ~DescriptorAllocator();

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

    std::shared_ptr<Device>         _device;
    VkDescriptorPool                _currentPool;
    PoolSizes                       _descriptorSizes;
    std::vector<VkDescriptorPool>   _usedPools;
    std::vector<VkDescriptorPool>   _freePools;
};

} // namespace bl