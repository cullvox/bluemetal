#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl
{

class DescriptorSetLayoutBinding : public BlockMembersBuilder
{
public:
    DescriptorSetLayoutBinding();
    ~DescriptorSetLayoutBinding() = default;

    DescriptorSetLayoutBinding& operator==(const DescriptorSetLayoutBinding& rhs) const;
    void SetLayout(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers);
    void AddStageFlags(VkShaderStageFlag flags);
    bool Compare(VkDescriptorType type, uint32_t descriptorCount) const;

private:
    uint32_t _location;
    VkDescriptorSetLayoutBinding _layout;
};

class DescriptorSetLayoutBuilder
{
public:
    DescriptorSetLayoutBuilder(uint32_t set);
    ~DescriptorSetLayoutBuilder();

    DescriptorSetLayoutBinding& operator[](uint32_t binding);
    VkDescriptorSetLayout Build(DescriptorSetLayoutCache* cache);
    uint32_t GetLocation() const;
    bool ContainsBinding(uint32_t binding) const;
    std::vector<DescriptorSetLayoutBinding> GetBindings() const;
    size_t HashBindings() const;
    VkDescriptorSetLayout GetLayout() const;

private:
    uint32_t _location;
    std::unordered_map<uint32_t, DescriptorSetLayoutBinding> _bindings;
    VkDescriptorSetLayout _layout;
};

} // namespace bl

namespace std 
{

template<> 
struct hash<bl::DescriptorSetLayoutBuilder>
{
    size_t operator()(const bl::DescriptorSetLayoutBuilder& desc) const noexcept
    {
        return desc.HashBindings();
    }
};

} // namespace std
