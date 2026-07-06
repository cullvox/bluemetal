#pragma once

#include "VulkanResource.h"

namespace bl
{

class VulkanDevice;
class VulkanDescriptorSetAllocatorCache;

class VulkanDescriptorSet : public VulkanResource
{
    VulkanDescriptorSetAllocatorCache* _cache;
    VkDescriptorSet _set;
    VkDescriptorSetLayout _layout;

public:
    VulkanDescriptorSet(VulkanDescriptorSetAllocatorCache* cache, VkDescriptorSetLayout layout, VkDescriptorSet set);
    VulkanDescriptorSet(VulkanDescriptorSet&& set);
    VulkanDescriptorSet(const VulkanDescriptorSet& set) = delete;
    ~VulkanDescriptorSet();

    VulkanDescriptorSet& operator=(VulkanDescriptorSet&& set);
    VulkanDescriptorSet& operator=(const VulkanDescriptorSet& set) = delete;

    VkDescriptorSet Get() const;
    VkDescriptorSetLayout GetLayout() const;
};


}