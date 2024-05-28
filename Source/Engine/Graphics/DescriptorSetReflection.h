#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "BlockReflection.h"

namespace bl
{

class DescriptorSetBindingReflection : public BlockReflection
{
public:
    DescriptorSetBindingReflection() = default;
    ~DescriptorSetBindingReflection() = default;

    bool operator==(const DescriptorSetBindingReflection& rhs) const;
    void SetBinding(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers);
    void SetName(const std::string& name);
    uint32_t GetLocation() const;
    void AddStageFlags(VkShaderStageFlags flags);
    bool Compare(VkDescriptorType type, uint32_t descriptorCount) const;
    VkDescriptorSetLayoutBinding GetBinding() const;
    std::string GetName() const;
    VkDescriptorType GetType() const;

private:
    std::string _name;
    VkDescriptorSetLayoutBinding _binding;
};

class DescriptorSetReflection
{
public:
    DescriptorSetReflection() = default;
    ~DescriptorSetReflection() = default;

    void SetLocation(uint32_t set);
    uint32_t GetLocation() const;
    bool ContainsBinding(uint32_t binding) const;
    std::vector<DescriptorSetBindingReflection> GetBindingReflections() const;
    std::vector<VkDescriptorSetLayoutBinding> GetSortedBindings() const;
    DescriptorSetBindingReflection& AccessOrInsertBinding(uint32_t binding);
    bool operator==(DescriptorSetReflection& rhs) const noexcept;

    // This could probably be cleaner but it is easier to reference the layout here when using materials.
    void SetLayout(VkDescriptorSetLayout layout);
    VkDescriptorSetLayout GetLayout();

private:
    uint32_t _set;
    std::unordered_map<uint32_t, DescriptorSetBindingReflection> _bindings;
    VkDescriptorSetLayout _layout;
};

} // namespace bl


