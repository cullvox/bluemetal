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
    VulkanDescriptorSetLayout* _layout;
    VkDescriptorSet _set;

    VulkanDescriptorSet();
public:
    VulkanDescriptorSet(VulkanDescriptorSetAllocatorCache* cache, VulkanDescriptorSetLayout* layout);
    VulkanDescriptorSet(VulkanDescriptorSet&& set);
    VulkanDescriptorSet(const VulkanDescriptorSet& set);
    ~VulkanDescriptorSet();

    VulkanDescriptorSet& operator=(VulkanDescriptorSet&& set);
    VulkanDescriptorSet& operator=(const VulkanDescriptorSet& set);

    VkDescriptorSet Get() const;
    const VulkanDescriptorSetLayout& GetLayout() const;
};


}