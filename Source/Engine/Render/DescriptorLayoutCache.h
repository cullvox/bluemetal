#pragma once

#include "Device.h"
#include "Shader.h"

namespace bl
{

class DescriptorLayoutCache
{
public:
    DescriptorLayoutCache(std::shared_ptr<Device> device);
    ~DescriptorLayoutCache();

    VkDescriptorSetLayout createDescriptorLayout(const VkDescriptorSetLayoutCreateInfo& createInfo);

private:
    struct DescriptorLayoutHash
    {
        size_t operator()(const DescriptorLayoutInfo& value) const
        {
            return value.hash();
        }
    };

    std::shared_ptr<Device> m_device;
    std::map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> m_cache;
};

} // namespace bl