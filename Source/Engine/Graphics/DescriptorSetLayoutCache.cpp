#include "DescriptorSetLayoutCache.h"

namespace bl 
{

DescriptorSetLayoutCache::DescriptorSetLayoutCache(Device* device)
    : _device(device)
{
}

DescriptorSetLayoutCache::~DescriptorSetLayoutCache()
{
    for (auto& pair : _cache)
        vkDestroyDescriptorSetLayout(_device->Get(), pair.second, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayoutCache::Acquire(DescriptorSetLay b)
{

    // Sort the bindings by the set binding.
    std::sort(b.begin(), b.end(),
        [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b){
            return a.binding < b.binding;
        });

    DescriptorSetLayoutBindings bindings{b};
    auto it = _cache.find(bindings);

    // If a same layout is already cached, use it, instead build a new layout.
    if (it != _cache.end()) 
    {
        // return found layout
        return (*it).second;
    } 
    else 
    {
        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.bindingCount = (uint32_t)b.size();
        createInfo.pBindings = b.data();

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        VK_CHECK(vkCreateDescriptorSetLayout(_device->Get(), &createInfo, nullptr, &layout))

        // add layout to cache
        _cache[bindings] = layout;
        return layout;
    }
}

} // namespace bl