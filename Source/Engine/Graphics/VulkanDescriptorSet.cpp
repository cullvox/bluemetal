#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanDescriptorSetLayout.h"
#include "GraphicsSystem.h"

namespace bl
{

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSetAllocatorCache* cache, VulkanDescriptorSetLayout* layout)
    : _cache(cache)
    , _layout(layout)
{
    cache->Allocate(*layout);
}

VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDescriptorSet& other)
{
    *this = other;
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
    _cache->FreeRaw(_layout->GetLayout(), _set);
}

VulkanDescriptorSet& VulkanDescriptorSet::operator=(const VulkanDescriptorSet& other)
{
    // Check if there's a preexisting set allocated here.
    if (_cache && _layout && _set)
        _cache->FreeRaw(_layout->GetLayout(), _set);

    _cache = other._cache;
    _layout = other._layout;
    _set = _cache->AllocateRaw(_layout->GetLayout());

    std::vector<VkCopyDescriptorSet> copies{_layout->GetBindings().size()};
    const auto bindings = _layout->GetBindings();

    for (int i = 0; i < _layout->GetBindings().size(); i++) {
        auto& c = copies[i];
        const auto& b = bindings[i];

        c.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        c.pNext = nullptr;
        c.srcSet = other._set;
        c.srcBinding = b.binding;
        c.srcArrayElement = 0;
        c.dstSet = _set;
        c.dstBinding = b.binding;
        c.dstArrayElement = 0;
        c.descriptorCount = b.descriptorCount;

        // Does not copy array elements yet. This could cause problems later.
        // Be warned.
    }

    auto device = GraphicsSystem::Get()->GetDevice();
    vkUpdateDescriptorSets(device->Get(), 0, nullptr, static_cast<uint32_t>(copies.size()), copies.data());

    return *this;
}

VulkanDescriptorSet& VulkanDescriptorSet::operator=(VulkanDescriptorSet&& set)
{
    if (this != &set)
    {
        _cache->FreeRaw(_layout->GetLayout(), _set);

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

const VulkanDescriptorSetLayout& VulkanDescriptorSet::GetLayout() const
{
    return *_layout;
}

}