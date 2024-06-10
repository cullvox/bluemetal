#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "BlockMeta.h"

namespace bl {

class DescriptorSetBindingMeta : public BlockMeta {
public:
    DescriptorSetBindingMeta() = default;
    ~DescriptorSetBindingMeta() = default;

    bool operator==(const DescriptorSetBindingMeta& rhs) const;
    bool Compare(VkDescriptorType type, uint32_t descriptorCount) const;

    DescriptorSetBindingMeta& SetBinding(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers);
    DescriptorSetBindingMeta& AddStageFlags(VkShaderStageFlags flags);
    DescriptorSetBindingMeta& SetType(VkDescriptorType type);

    uint32_t GetLocation() const;
    VkDescriptorType GetType() const;
    VkDescriptorSetLayoutBinding GetBinding() const;
    bool IsBlock() const; /** @brief Returns true if the binding uses uniform storage. */

private:
    VkDescriptorSetLayoutBinding _binding;
};

class DescriptorSetReflection {
public:
    DescriptorSetReflection() = default;
    ~DescriptorSetReflection() = default;

    bool operator==(DescriptorSetReflection& rhs) const noexcept;
    DescriptorSetBindingMeta& operator[](uint32_t binding);
    bool Contains(uint32_t binding) const;

    DescriptorSetReflection& SetLocation(uint32_t set);

    uint32_t GetLocation() const;
    std::vector<DescriptorSetBindingMeta> GetMetaBindings() const; /** @brief Returns all the underlying metadata about descriptor sets. */
    std::vector<VkDescriptorSetLayoutBinding> GetSortedBindings() const; /** @brief Returns underlying vulkan descriptor data, use for descriptor set layout creation/retrieving cached. */
    std::unordered_map<uint32_t, DescriptorSetBindingMeta>& GetBindings();
    const std::unordered_map<uint32_t, DescriptorSetBindingMeta>& GetBindings() const;

private:
    uint32_t _set;
    std::unordered_map<uint32_t, DescriptorSetBindingMeta> _bindings;
};

} // namespace bl


