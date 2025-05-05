#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "VulkanReflectedBlock.h"

namespace bl {

class VulkanReflectedDescriptorSetBinding : public VulkanReflectedBlock {
public:
    VulkanReflectedDescriptorSetBinding() = default;
    ~VulkanReflectedDescriptorSetBinding() = default;

    bool operator==(const VulkanReflectedDescriptorSetBinding& rhs) const;
    bool Compare(VkDescriptorType type, uint32_t descriptorCount) const;

    VulkanReflectedDescriptorSetBinding& SetBinding(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers);
    VulkanReflectedDescriptorSetBinding& AddStageFlags(VkShaderStageFlags flags);
    VulkanReflectedDescriptorSetBinding& SetType(VkDescriptorType type);

    uint32_t GetLocation() const;
    VkDescriptorType GetType() const;
    VkDescriptorSetLayoutBinding GetBinding() const;
    bool IsBlock() const; /** @brief Returns true if the binding uses uniform storage. */

private:
    VkDescriptorSetLayoutBinding _binding;
};

class VulkanReflectedDescriptorSet {
public:
    VulkanReflectedDescriptorSet() = default;
    ~VulkanReflectedDescriptorSet() = default;

    bool operator==(VulkanReflectedDescriptorSet& rhs) const noexcept;
    VulkanReflectedDescriptorSetBinding& operator[](uint32_t binding);
    bool Contains(uint32_t binding) const;

    VulkanReflectedDescriptorSet& SetLocation(uint32_t set);

    uint32_t GetLocation() const;
    std::vector<VulkanReflectedDescriptorSetBinding> GetMetaBindings() const; /** @brief Returns all the underlying metadata about descriptor sets. */
    std::vector<VkDescriptorSetLayoutBinding> GetSortedBindings() const; /** @brief Returns underlying vulkan descriptor data, use for descriptor set layout creation/retrieving cached. */
    std::unordered_map<uint32_t, VulkanReflectedDescriptorSetBinding>& GetBindings();
    const std::unordered_map<uint32_t, VulkanReflectedDescriptorSetBinding>& GetBindings() const;

private:
    uint32_t _set;
    std::unordered_map<uint32_t, VulkanReflectedDescriptorSetBinding> _bindings;
};

} // namespace bl


