#include "VulkanDescriptorSetReflection.h"

namespace bl {

////////// DescriptorSetBindingMeta //////////

bool VulkanDescriptorSetBindingReflection::operator==(const VulkanDescriptorSetBindingReflection& rhs) const {
    return _binding.binding == rhs._binding.binding
        && _binding.descriptorType == rhs._binding.descriptorType
        && _binding.descriptorCount == rhs._binding.descriptorCount;

// Stage flags aren't a very useful metric for determining 
// equality here because the stage *could* be added later.

    //  && _binding.stageFlags != rhs._binding.stageFlags;
}

bool VulkanDescriptorSetBindingReflection::Compare(VkDescriptorType type, uint32_t descriptorCount) const {
    return _binding.descriptorType == type 
        && _binding.descriptorCount == descriptorCount;
}

VulkanDescriptorSetBindingReflection& VulkanDescriptorSetBindingReflection::SetBinding(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers) {
    _binding.binding = binding;
    _binding.descriptorType = type;
    _binding.descriptorCount = descriptorCount;
    _binding.stageFlags = flags;
    _binding.pImmutableSamplers = immutableSamplers;
    return *this;
}

VulkanDescriptorSetBindingReflection& VulkanDescriptorSetBindingReflection::AddStageFlags(VkShaderStageFlags flags) {
    _binding.stageFlags |= flags;
    return *this;
}

VulkanDescriptorSetBindingReflection& VulkanDescriptorSetBindingReflection::SetType(VkDescriptorType type) {
    _binding.descriptorType = type;
    return *this;
}

uint32_t VulkanDescriptorSetBindingReflection::GetLocation() const {
    return _binding.binding;
}

VkDescriptorSetLayoutBinding VulkanDescriptorSetBindingReflection::GetBinding() const {
    return _binding;
}

VkDescriptorType VulkanDescriptorSetBindingReflection::GetType() const {
    return _binding.descriptorType;
}

bool VulkanDescriptorSetBindingReflection::IsBlock() const {
    return _binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER 
        || _binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
}

////////// DescriptorSetReflection //////////

VulkanDescriptorSetBindingReflection& VulkanDescriptorSetReflection::operator[](uint32_t binding) {
    return _bindings[binding];
}

bool VulkanDescriptorSetReflection::operator==(VulkanDescriptorSetReflection& rhs) const noexcept {
    return _set == rhs._set && _bindings == rhs._bindings;
}

VulkanDescriptorSetReflection& VulkanDescriptorSetReflection::SetLocation(uint32_t set) {
    _set = set;
    return *this;
}

uint32_t VulkanDescriptorSetReflection::GetLocation() const {
    return _set;
}

bool VulkanDescriptorSetReflection::Contains(uint32_t binding) const {
    return _bindings.contains(binding);
}

std::vector<VulkanDescriptorSetBindingReflection> VulkanDescriptorSetReflection::GetMetaBindings() const {
    // Create a vector containing each binding from the hash map.
    std::vector<VulkanDescriptorSetBindingReflection> bindings;
    bindings.reserve(_bindings.size());

    for (const auto& pair : _bindings) {
        bindings.push_back(pair.second);
    }

    // Sort the bindings by location in ascending order.
    std::sort(bindings.begin(), bindings.end(), [](const auto& left, const auto& right){ return left.GetLocation() < right.GetLocation(); });

    return bindings;
}

std::vector<VkDescriptorSetLayoutBinding> VulkanDescriptorSetReflection::GetSortedBindings() const {
    std::vector<VkDescriptorSetLayoutBinding> out;
    out.reserve(_bindings.size());

    auto reflections = GetMetaBindings();
    for (const auto& binding : reflections) {
        out.push_back(binding.GetBinding());
    }

    return out;
}

std::unordered_map<uint32_t, VulkanDescriptorSetBindingReflection>& VulkanDescriptorSetReflection::GetBindings() {
    return _bindings;
}

const std::unordered_map<uint32_t, VulkanDescriptorSetBindingReflection>& VulkanDescriptorSetReflection::GetBindings() const {
    return _bindings;
}

}
