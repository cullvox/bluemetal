#include "DescriptorSetReflection.h"

namespace bl
{

////////// DescriptorSetBindingReflection //////////

bool DescriptorSetBindingReflection::operator==(const DescriptorSetBindingReflection& rhs) const
{
    return _binding.binding == rhs._binding.binding
        && _binding.descriptorType == rhs._binding.descriptorType
        && _binding.descriptorCount == rhs._binding.descriptorCount;

// Stage flags aren't a very useful metric for determining 
// equality here because the stage *could* be added later.

    //  && _binding.stageFlags != rhs._binding.stageFlags;
}

void DescriptorSetBindingReflection::SetBinding(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers)
{
    _binding.binding = binding;
    _binding.descriptorType = type;
    _binding.descriptorCount = descriptorCount;
    _binding.stageFlags = flags;
    _binding.pImmutableSamplers = immutableSamplers;
}

void DescriptorSetBindingReflection::SetName(const std::string& name)
{
    _name = name;
}

uint32_t DescriptorSetBindingReflection::GetLocation() const
{
    return _binding.binding;
}

void DescriptorSetBindingReflection::AddStageFlags(VkShaderStageFlags flags)
{
    _binding.stageFlags |= flags;
}

bool DescriptorSetBindingReflection::Compare(VkDescriptorType type, uint32_t descriptorCount) const
{
    return _binding.descriptorType == type 
        && _binding.descriptorCount == descriptorCount;
}

VkDescriptorSetLayoutBinding DescriptorSetBindingReflection::GetBinding() const
{
    return _binding;
}

VkDescriptorType DescriptorSetBindingReflection::GetType() const
{
    return _binding.descriptorType;
}

////////// DescriptorSetReflection //////////

void DescriptorSetReflection::SetLocation(uint32_t set)
{
    _set = set;
}

uint32_t DescriptorSetReflection::GetLocation() const
{
    return _set;
}

bool DescriptorSetReflection::ContainsBinding(uint32_t binding) const
{
    return _bindings.contains(binding);
}

std::vector<DescriptorSetBindingReflection> DescriptorSetReflection::GetBindingReflections() const
{
    // Create a vector containing each binding from the hash map.
    std::vector<DescriptorSetBindingReflection> bindings;
    bindings.reserve(_bindings.size());

    for (const auto& pair : _bindings)
        bindings.push_back(pair.second);

    // Sort the bindings by location in ascending order.
    std::sort(bindings.begin(), bindings.end(), 
        [](const auto& left, const auto& right){ return left.GetLocation() < right.GetLocation(); });

    return bindings;
}

std::vector<VkDescriptorSetLayoutBinding> DescriptorSetReflection::GetSortedBindings() const
{
    std::vector<VkDescriptorSetLayoutBinding> out;
    out.reserve(_bindings.size());

    auto reflections = GetBindingReflections();
    for (const auto& binding : reflections)
        out.push_back(binding.GetBinding());

    return out;
}

DescriptorSetBindingReflection& DescriptorSetReflection::AccessOrInsertBinding(uint32_t binding)
{
    return _bindings[binding];
}

bool DescriptorSetReflection::operator==(DescriptorSetReflection& rhs) const noexcept
{
    return _set == rhs._set && _bindings == rhs._bindings;
}

void DescriptorSetReflection::SetLayout(VkDescriptorSetLayout layout)
{
    _layout = layout;
}

VkDescriptorSetLayout DescriptorSetReflection::GetLayout()
{
    return _layout;
}

}
