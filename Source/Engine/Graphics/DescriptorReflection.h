#pragma once

#include "Precompiled.h"
#include "Core/Hash.h"
#include "Vulkan.h"

namespace bl
{

class DescriptorSetLayoutBinding
{
public:
    DescriptorSetLayoutBinding();
    ~DescriptorSetLayoutBinding() = default;

    void SetLayout(uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags flags, const VkSampler* immutableSamplers);
    void AddStageFlags(VkShaderStageFlag flags);
    BlockMember& AccessOrInsertMember(std::string name);
    std::vector<BlockMember> GetMembers() const;
    bool Compare(VkDescriptorType type, uint32_t descriptorCount) const;

    BlockMember& operator[](std::string name); // Shorthanded for AccessOrInsertMember().
    DescriptorSetLayoutBinding& operator==(const DescriptorSetLayoutBinding& rhs) const;

private:
    uint32_t _location;
    VkDescriptorSetLayoutBinding _layout;
    std::unordered_map<std::string, BlockMember> _members;
};

class DescriptorSetLayoutBuilder
{
public:
    DescriptorSetLayoutBuilder(uint32_t set);
    ~DescriptorSetLayoutBuilder();

    uint32_t GetLocation() const;
    DescriptorBindingReflection& AccessOrInsertBinding(uint32_t location);
    bool ContainsBinding(uint32_t binding) const;
    std::vector<DescriptorSetLayoutBinding> GetBindings() const;
    size_t HashBindings() const;
    void Build(DescriptorSetLayoutCache* cache);

    DescriptorSetLayoutBinding& operator[](uint32_t binding); // Shorthanded for AccessOrInsertBinding().

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
