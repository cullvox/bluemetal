#pragma once

#include "VulkanResource.h"

namespace bl
{

class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanDescriptorSetAllocatorCache;

class VulkanDescriptorSet : public VulkanResource
{
    VulkanDescriptorSetAllocatorCache* _cache;
    const VulkanDescriptorSetLayout* _layout;
    VkDescriptorSet _set;

public:
    VulkanDescriptorSet();
    VulkanDescriptorSet(VulkanDescriptorSetAllocatorCache* cache, const VulkanDescriptorSetLayout* layout);
    VulkanDescriptorSet(VulkanDescriptorSet&& set);
    VulkanDescriptorSet(const VulkanDescriptorSet& set);
    ~VulkanDescriptorSet();

    VulkanDescriptorSet& operator=(VulkanDescriptorSet&& set);
    VulkanDescriptorSet& operator=(const VulkanDescriptorSet& set);

    VkDescriptorSet Get() const;
    const VulkanDescriptorSetLayout& GetLayout() const;
};


}