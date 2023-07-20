#include "DescriptorLayoutCache.h"

blDescriptorLayoutCache::blDescriptorLayoutCache(std::shared_ptr<blDevice> device)
    : _device(device)
{
}

blDescriptorLayoutCache::~blDescriptorLayoutCache()
{
    for (auto pair : _cache)
    {
        vkDestroyDescriptorSetLayout(_device->getDevice(), pair.second, nullptr);
    }
}

VkDescriptorSetLayout blDescriptorLayoutCache::createDescriptorLayout(const VkDescriptorSetLayoutCreateInfo& createInfo)
{
    blDescriptorLayoutInfo info;
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
    auto it = _cache.find(info);

    if (it != _cache.end())
    {
        // return found layout
        return (*it).second;
    }
    else
    {
        // create a new descriptor set layout
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        if (vkCreateDescriptorSetLayout(_device->getDevice(), &createInfo, nullptr, &layout) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not crate a Vulkan descriptor set layout!");
        }

        // add layout to cache
        _cache[info] = layout;
        
        return layout;
    }
}

