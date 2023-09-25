#include "DescriptorLayoutCache.h"

namespace bl {

bool DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
{
    if (other.bindings.size() != bindings.size()) {
        return false;
    } else {
        for (uint32_t i = 0; i < bindings.size(); i++) {
            if (other.bindings[i].binding != bindings[i].binding)
                return false;
            if (other.bindings[i].descriptorType != bindings[i].descriptorType)
                return false;
            if (other.bindings[i].descriptorCount != bindings[i].descriptorCount)
                return false;
            if (other.bindings[i].stageFlags != bindings[i].stageFlags)
                return false;
        }
    }

    return true;
}

size_t DescriptorLayoutInfo::hash() const
{
    using std::hash;
    using std::size_t;

    size_t result = hash<size_t>()(bindings.size());

    for (const VkDescriptorSetLayoutBinding& b : bindings) {
        // pack binding into a uint64
        size_t bindingHash
            = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

        // shuffle data and xor with the main hash
        result ^= hash<size_t>()(bindingHash);
    }

    return result;
}

DescriptorLayoutCache::DescriptorLayoutCache(GraphicsDevice* pDevice)
    : m_pDevice(pDevice)
{
}

DescriptorLayoutCache::~DescriptorLayoutCache()
{
    for (auto pair : m_cache) {
        vkDestroyDescriptorSetLayout(m_pDevice->getHandle(), pair.second, nullptr);
    }
}

VkDescriptorSetLayout DescriptorLayoutCache::createLayout(
    const VkDescriptorSetLayoutCreateInfo& createInfo)
{
    DescriptorLayoutInfo info;
    info.bindings.resize(createInfo.bindingCount);

    // add bindings to our info
    for (uint32_t i = 0; i < createInfo.bindingCount; i++) {
        info.bindings.push_back(createInfo.pBindings[i]);
    }

    // sort bindings
    std::sort(info.bindings.begin(), info.bindings.end(),
        [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) {
            return a.binding < b.binding;
        });

    // look for descriptor set in cache
    auto it = m_cache.find(info);

    if (it != m_cache.end()) {
        // return found layout
        return (*it).second;
    } else {
        // create a new descriptor set layout
        VkDescriptorSetLayout layout = VK_NULL_HANDLE;
        if (vkCreateDescriptorSetLayout(m_pDevice->getHandle(), &createInfo, nullptr, &layout)
            != VK_SUCCESS) {
            throw std::runtime_error("Could not crate a Vulkan descriptor set layout!");
        }

        // add layout to cache
        m_cache[info] = layout;

        return layout;
    }
}

} // namespace bl