#pragma once

namespace bl
{

class VulkanDescriptorSetLayout
{
    VkDescriptorSetLayout _layout;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    VulkanDescriptorSetLayout();
public:
    VulkanDescriptorSetLayout(std::span<VkDescriptorSetLayoutBinding> _bindings);
    VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&) = default;

    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;
    VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&&) = default;

    VkDescriptorSetLayout GetLayout() const;
    std::span<const VkDescriptorSetLayoutBinding> GetBindings() const;

};

}