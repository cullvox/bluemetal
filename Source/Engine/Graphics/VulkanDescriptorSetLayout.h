#pragma once

namespace bl
{

class VulkanDescriptorSetLayoutCache;

class VulkanDescriptorSetLayout
{
    VkDescriptorSetLayout _layout;
    VulkanDescriptorSetLayoutCache* _cache;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

public:
    VulkanDescriptorSetLayout();
    VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&) = default;
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayoutCache* cache, std::span<VkDescriptorSetLayoutBinding> _bindings);
    ~VulkanDescriptorSetLayout();

    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;
    VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&&) = default;

    VkDescriptorSetLayout GetLayout() const;
    std::span<const VkDescriptorSetLayoutBinding> GetBindings() const;
};

}