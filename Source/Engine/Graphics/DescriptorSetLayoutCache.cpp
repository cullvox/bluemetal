#include "Device.h"
#include "DescriptorSetLayoutCache.h"

namespace bl 
{

bool DescriptorLayoutCacheData::operator==(const DescriptorLayoutCacheData& rhs) const
{
    for (uint32_t i = 0; i < bindings.size(); i++)
    {
        const auto& b = bindings[i];
        const auto& rb = rhs.bindings[i];

        if (b.binding != rb.binding
            || b.descriptorType != rb.descriptorType
            || b.descriptorCount != rb.descriptorCount
            || b.stageFlags != rb.stageFlags) return false;
    }

    return true;
}

size_t DescriptorLayoutCacheHasher::operator()(const DescriptorLayoutCacheData& data) const noexcept
{
    const auto& bindings = data.bindings;
    size_t seed = std::hash<size_t>()(bindings.size());
    for (const auto& b : bindings) 
    {
        size_t packed = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24; // pack binding into a uint64
        seed = bl::hash_combine(seed, packed); // shuffle data and xor with the main hash
    }
    return seed;
}

DescriptorSetLayoutCache::DescriptorSetLayoutCache(Device* device)
    : _device(device)
{
}

DescriptorSetLayoutCache::~DescriptorSetLayoutCache()
{
    for (auto& pair : _cache)
        vkDestroyDescriptorSetLayout(_device->Get(), pair.second, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayoutCache::Acquire(std::span<VkDescriptorSetLayoutBinding> bindings)
{
    DescriptorLayoutCacheData data{};
    data.bindings.assign(bindings.begin(), bindings.end());

    auto it = _cache.find(data);
    if (it != _cache.end())
    {
        return (*it).second;
    } 

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = (uint32_t)data.bindings.size();
    createInfo.pBindings = data.bindings.data();

    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorSetLayout(_device->Get(), &createInfo, nullptr, &layout))

    _cache[data] = layout;
    return layout;
}

} // namespace bl