#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanDescriptorSetLayout.h"
#include "GraphicsSystem.h"

namespace bl
{

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSetAllocatorCache* cache, VulkanDescriptorSetLayout layout)
    : _cache(cache)
    , _layout(layout)
{
    cache->Allocate(layout);
}

VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDescriptorSet& other)
{
    *this = other._cache->Allocate(*other._layout);
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
    _cache->Free(this);
}

VulkanDescriptorSet& VulkanDescriptorSet::operator=(const VulkanDescriptorSet& other)
{
    // Check if there's a preexisting set allocated here.
    if (_cache && _layout && _set)
        _cache->FreeRaw(_layout->GetLayout(), _set);

    _cache = other._cache;
    _layout = other._layout;

    std::vector<VkCopyDescriptorSet> copy{_layout->GetBindings().size()};
    const auto bindings = _layout->GetBindings();

    for (int i = 0; i < _layout->GetBindings().size(); i++) {
        auto& c = copy[i];
        const auto& b = bindings[i];

        copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        copy.pNext = nullptr;
        copy.srcSet = other._set;
        copy.srcBinding = b.binding;
        copy.srcArrayElement = 0;
        copy.
    }

    VkCopyDescriptorSet copy;
    copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
    copy.pNext = nullptr;
    copy.srcSet = other._set;
    copy.

    auto device = GraphicsSystem::Get()->GetDevice();
    vkUpdateDescriptorSets(device, 0, nullptr, 1, &copy);
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