#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayoutCache.h"

namespace bl {

bool VulkanDescriptorLayoutCacheData::operator==(const VulkanDescriptorLayoutCacheData& rhs) const {
    for (uint32_t i = 0; i < bindings.size(); i++) {
        const auto& b = bindings[i];
        const auto& rb = rhs.bindings[i];

        if (b.binding != rb.binding
            || b.descriptorType != rb.descriptorType
            || b.descriptorCount != rb.descriptorCount
            || b.stageFlags != rb.stageFlags) return false;
    }

    return true;
}

size_t VulkanDescriptorLayoutCacheHasher::operator()(const VulkanDescriptorLayoutCacheData& data) const noexcept {
    const auto& bindings = data.bindings;
    size_t seed = std::hash<size_t>()(bindings.size());
    for (const auto& b : bindings) {
        size_t packed = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24; // pack binding into a uint64
        bl::hash_combine(seed, packed); // shuffle data and xor with the main hash
    }
    return seed;
}

VulkanDescriptorSetLayoutCache::VulkanDescriptorSetLayoutCache(const VulkanDevice* device)
    : _device(device) { }

VulkanDescriptorSetLayoutCache::~VulkanDescriptorSetLayoutCache() {
    // Destroy all descriptor set layouts.
    for (auto& pair : _cache)
        vkDestroyDescriptorSetLayout(_device->Get(), pair.second, nullptr);
}

VkDescriptorSetLayout VulkanDescriptorSetLayoutCache::Acquire(std::span<VkDescriptorSetLayoutBinding> bindings) {
    VulkanDescriptorLayoutCacheData data;
    data.bindings.assign(bindings.begin(), bindings.end());

    // Ensure that the bindings are sorted before they are hashed.
    std::sort(data.bindings.begin(), data.bindings.end(), [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b){ return a.binding < b.binding; });

    auto it = _cache.find(data);
    if (it != _cache.end())
        return (*it).second; // Return an already cached descriptor set layout.

    // Create a new descriptor set layout, cache it, and return it.
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = (uint32_t)bindings.size();
    createInfo.pBindings = bindings.data();

    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorSetLayout(_device->Get(), &createInfo, nullptr, &layout))

    _cache[data] = layout;
    return layout;
}

} // namespace bl