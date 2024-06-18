#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "VulkanBlockReflection.h"

namespace bl {

class VulkanDescriptorSetBindingReflection : public VulkanBlockReflection {
public:
    VulkanDescriptorSetBindingReflection() = default;
    ~VulkanDescriptorSetBindingReflection() = default;

    bool operator==(const VulkanDescriptorSetBindingReflection& rhs) const;
    bool Compare(VkDescriptorType type, uint32_t descriptorCount) const;

    VulkanDescriptorSetBindingReflection& SetBinding(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers);
    VulkanDescriptorSetBindingReflection& AddStageFlags(VkShaderStageFlags flags);
    VulkanDescriptorSetBindingReflection& SetType(VkDescriptorType type);

    uint32_t GetLocation() const;
    VkDescriptorType GetType() const;
    VkDescriptorSetLayoutBinding GetBinding() const;
    bool IsBlock() const; /** @brief Returns true if the binding uses uniform storage. */

private:
    VkDescriptorSetLayoutBinding _binding;
};

class VulkanDescriptorSetReflection {
public:
    VulkanDescriptorSetReflection() = default;
    ~VulkanDescriptorSetReflection() = default;

    bool operator==(VulkanDescriptorSetReflection& rhs) const noexcept;
    VulkanDescriptorSetBindingReflection& operator[](uint32_t binding);
    bool Contains(uint32_t binding) const;

    VulkanDescriptorSetReflection& SetLocation(uint32_t set);

    uint32_t GetLocation() const;
    std::vector<VulkanDescriptorSetBindingReflection> GetMetaBindings() const; /** @brief Returns all the underlying metadata about descriptor sets. */
    std::vector<VkDescriptorSetLayoutBinding> GetSortedBindings() const; /** @brief Returns underlying vulkan descriptor data, use for descriptor set layout creation/retrieving cached. */
    std::unordered_map<uint32_t, VulkanDescriptorSetBindingReflection>& GetBindings();
    const std::unordered_map<uint32_t, VulkanDescriptorSetBindingReflection>& GetBindings() const;

private:
    uint32_t _set;
    std::unordered_map<uint32_t, VulkanDescriptorSetBindingReflection> _bindings;
};

} // namespace bl


