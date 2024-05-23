#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl
{

struct DescriptorLayoutCacheData
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bool operator==(const DescriptorLayoutCacheData& rhs) const;
};

struct DescriptorLayoutCacheHasher
{
    size_t operator()(const DescriptorLayoutCacheData& data) const noexcept;
};

class Device;
class DescriptorSetLayoutCache
{
public:
    DescriptorSetLayoutCache(Device* device);
    ~DescriptorSetLayoutCache();

    VkDescriptorSetLayout Acquire(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

private:
    Device* _device;
    std::unordered_map<DescriptorLayoutCacheData, VkDescriptorSetLayout, DescriptorLayoutCacheHasher> _cache;
};

} // namespace bl
