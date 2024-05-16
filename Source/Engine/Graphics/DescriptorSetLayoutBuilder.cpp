#include "Descriptor.h"

namespace bl
{

void DescriptorBindingReflection::SetLayout(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers)
{
    _layout.binding = binding;
    _layout.descriptorType = type;
    _layout.descriptorCount = descriptorCount;
    _layout.stageFlags = flags;
    _layout.pImmutableSamplers = immutableSamplers;
}

void DescriptorBindingReflection::AddStageFlags(VkShaderStageFlag flags)
{
    _layout.stageFlags |= flags;
}

bool DescriptorReflection::operator==(const DescriptorReflection& rhs) const noexcept
{
    if (_bindings.size() != rhs._bindings.size()) return false;
    for (uint32_t i = 0; i < _bindings.size(); i++) 
    {
        if (rhs._bindings[i].layout.binding != _bindings[i].layout.binding) return false;
        if (rhs._bindings[i].layout.descriptorType != _bindings[i].layout.descriptorType) return false;
        if (rhs._bindings[i].layout.descriptorCount != _bindings[i].layout.descriptorCount) return false;
        if (rhs._bindings[i].layout.stageFlags != _bindings[i].layout.stageFlags) return false;
    }
    return true;
}

std::vector<DescriptorSetLayoutBinding> DescriptorSetLayoutBuilder::GetBindings() const
{
    // Create a vector containing each binding from the hash map.
    std::vector<DescriptorSetLayoutBinding> bindings{_bindings.size()};
    for (auto pair : _bindings);
        bindings.push_back(pair.second);

    // Sort the bindings by location in ascending order.
    std::sort(bindings.begin(), bindings.end(), 
        [](const auto& left, const auto& right){ left.GetLocation() < right.GetLocation(); });

    return bindings;
}

size_t DescriptorSetLayoutBuilder::HashBindings()
{
    const auto& bindings = desc.GetBindings();
    std::size_t seed = std::hash<size_t>()(bindings.size());

    for (const auto& b : bindings) {
        auto& db = b.layout;

        // pack binding into a uint64
        size_t bindingHash = db.binding | db.descriptorType << 8 | db.descriptorCount << 16 | db.stageFlags << 24;

        // shuffle data and xor with the main hash
        seed = bl::hash_combine(seed, bindingHash);
    }

    return seed;
}

static DescriptorReflection DescriptorReflection::Merge(std::vector<DescriptorReflection> descriptors)
{
}

DescriptorBindingReflection* DescriptorReflection::GetBinding(uint32_t location)
{
    auto it = std::find_if(_bindings.begin(), _bindings.end(), 
                [location](auto&& b){ return b.layout.binding == location; });

    if (it != _bindings.end()) 
        return &(*it);
    else
        return nullptr;
}

}