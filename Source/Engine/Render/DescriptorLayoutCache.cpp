#include "DescriptorLayoutCache.h"

namespace bl
{

DescriptorLayoutCache::DescriptorLayoutCache(std::shared_ptr<Device> device)
    : m_device(device)
{
}

DescriptorLayoutCache::~DescriptorLayoutCache()
{
    for (auto pair : m_cache)
    {
        vkDestroyDescriptorSetLayout(m_device->getDevice(), pair.second, nullptr);
    }
}

VkDescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(const VkDescriptorSetLayoutCreateInfo& createInfo)
{
    DescriptorLayoutInfo info;
    info.bindings.resize(createInfo.bindingCount);

    // add bindings to our info
    for (uint32_t i = 0; i < createInfo.bindingCount; i++)
    {
        info.bindings.push_back(createInfo.pBindings[i]);
    }

    // sort bindings
    std::sort(info.bindings.begin(), info.bindings.end(), 
        [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b)
        {
            return a.binding < b.binding;
        });

    // look for descriptor set in cache
    auto it = m_cache.find(info);

    if (it != m_cache.end())
    {
        // return found layout
        return (*it).second;
    }
    else
    {
        // create a new descriptor set layout
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        if (vkCreateDescriptorSetLayout(m_device->getDevice(), &createInfo, nullptr, &layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not crate a Vulkan descriptor set layout!");
        }

        // add layout to cache
        m_cache[info] = layout;
        
        return layout;
    }
}

} // namespace bl