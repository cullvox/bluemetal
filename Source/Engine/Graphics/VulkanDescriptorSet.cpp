#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetAllocatorCache.h"

namespace bl
{

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSetAllocatorCache* cache, VkDescriptorSetLayout layout, VkDescriptorSet set)
    : _cache(cache)
    , _layout(layout)
    , _set(set)
{
}

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSet&& set)
    : _cache(set._cache)
    , _layout(set._layout)
    , _set(set._set)
{
    set._cache = nullptr;
    set._layout = VK_NULL_HANDLE;
    set._set = VK_NULL_HANDLE;
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    _cache->Free(_layout, _set);
}

VulkanDescriptorSet& VulkanDescriptorSet::operator=(VulkanDescriptorSet&& set)
{
    if (this != &set)
    {
        _cache->Free(_layout, _set);

        _cache = set._cache;
        _layout = set._layout;
        _set = set._set;

        set._cache = nullptr;
        set._layout = VK_NULL_HANDLE;
        set._set = VK_NULL_HANDLE;
    }

    return *this;
}

VkDescriptorSet VulkanDescriptorSet::Get() const
{
    return _set;
}

VkDescriptorSetLayout VulkanDescriptorSet::GetLayout() const
{
    return _layout;
}

}