#pragma once

#include "Core/Hash.h"
#include "Graphics/Device.h"

#include "Descriptor.h"

namespace bl
{

class DescriptorSetLayoutCache
{
public:
    DescriptorSetLayoutCache(Device* device);
    ~DescriptorSetLayoutCache();

    VkDescriptorSetLayout Acquire(std::vector<VkDescriptorSetLayoutBinding> bindings);

private:
    Device* _device;
    std::unordered_map<DescriptorReflection, VkDescriptorSetLayout> _cache;
};

} // namespace bl
 