#pragma once

#include "Device.h"
#include "Shader.h"

class blDescriptorLayoutCache
{
public:
    explicit blDescriptorLayoutCache(std::shared_ptr<blDevice> device);
    ~blDescriptorLayoutCache();

    VkDescriptorSetLayout createDescriptorLayout(const VkDescriptorSetLayoutCreateInfo& createInfo);

private:
    struct blDescriptorLayoutHash
    {
        size_t operator()(const blDescriptorLayoutInfo& value) const
        {
            return value.hash();
        }
    };

    std::shared_ptr<blDevice> _device;
    std::map<blDescriptorLayoutInfo, VkDescriptorSetLayout, blDescriptorLayoutHash> _cache;
};