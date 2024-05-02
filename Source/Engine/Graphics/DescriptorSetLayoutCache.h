#pragma once

#include "Core/Hash.h"
#include "Graphics/Device.h"

namespace bl
{

struct DescriptorSetLayoutBindings
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    bool operator==(const bl::DescriptorSetLayoutBindings& rhs) const noexcept
    {
        if (bindings.size() != bindings.size()) return false;
            
        for (uint32_t i = 0; i < bindings.size(); i++) {
            if (rhs.bindings[i].binding != bindings[i].binding) return false;
            if (rhs.bindings[i].descriptorType != bindings[i].descriptorType) return false;
            if (rhs.bindings[i].descriptorCount != bindings[i].descriptorCount) return false;
            if (rhs.bindings[i].stageFlags != bindings[i].stageFlags) return false;
        }

        return true;
    }

    bool EqualDisregardingStage(const bl::DescriptorSetLayoutBindings& rhs) const noexcept
    {
        if (bindings.size() != bindings.size()) return false;
            
        for (uint32_t i = 0; i < bindings.size(); i++) {
            if (rhs.bindings[i].binding != bindings[i].binding) return false;
            if (rhs.bindings[i].descriptorType != bindings[i].descriptorType) return false;
            if (rhs.bindings[i].descriptorCount != bindings[i].descriptorCount) return false;
            // if (rhs.bindings[i].stageFlags != bindings[i].stageFlags) return false;
        }

        return true;
    }
};

struct DescriptorSetLayoutBindingsHash
{
    std::size_t operator()(const bl::DescriptorSetLayoutBindings& bindings) const noexcept
    {
        std::size_t seed = std::hash<size_t>()(bindings.bindings.size());

        for (const VkDescriptorSetLayoutBinding& b : bindings.bindings) {
            // pack binding into a uint64
            size_t bindingHash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

            // shuffle data and xor with the main hash
            seed = hash_combine(seed, bindingHash);
        }

        return seed;
    }
};

class DescriptorSetLayoutCache
{
public:
    DescriptorSetLayoutCache(Device* device);
    ~DescriptorSetLayoutCache();

    VkDescriptorSetLayout acquire(std::vector<VkDescriptorSetLayoutBinding> bindings);

private:
    Device* _device;
    std::unordered_map<DescriptorSetLayoutBindings, VkDescriptorSetLayout, DescriptorSetLayoutBindingsHash> _cache;
};

} // namespace bl
 