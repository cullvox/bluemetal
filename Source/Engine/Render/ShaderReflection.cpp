#include "ShaderReflection.h"

namespace bl
{

bool DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
{
    if (other.bindings.size() != bindings.size())
    {
        return false;
    }
    else
    {
        for (uint32_t i = 0; i < bindings.size(); i++)
        {
            if (other.bindings[i].binding != bindings[i].binding) return false;
            if (other.bindings[i].descriptorType != bindings[i].descriptorType) return false;
            if (other.bindings[i].descriptorCount != bindings[i].descriptorCount) return false;
            // if (other.bindings[i].stageFlags != bindings[i].stageFlags) return false;
        }
    }

    return true;
}

size_t DescriptorLayoutInfo::hash() const
{
    using std::size_t;
    using std::hash;

    size_t result = hash<size_t>()(bindings.size());

    for (const VkDescriptorSetLayoutBinding& b : bindings)
    {
        // pack binding into a uint64
        size_t bindingHash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16; // | b.stageFlags << 24;

        // shuffle data and xor with the main hash
        result ^= hash<size_t>()(bindingHash);
    }

    return result;
}




} // namespace bl