#include "VulkanReflectedBlock.h"

namespace bl {

VulkanVariableBlock& VulkanVariableBlock::SetBinding(uint32_t binding) {
    _binding = binding;
    return *this;
}

VulkanVariableBlock& VulkanVariableBlock::SetType(VulkanVariableBlockType type) {
    _type = type;
    return *this;
}

VulkanVariableBlock& VulkanVariableBlock::SetOffset(uint32_t offset) {
    _offset = offset;
    return *this;
}

VulkanVariableBlock& VulkanVariableBlock::SetSize(uint32_t size) {
    _size = size;
    return *this;
}

VulkanVariableBlock& VulkanVariableBlock::SetName(const std::string& name) {
    _name = name;
    return *this;
}

uint32_t VulkanVariableBlock::GetBinding() const {
    return _binding;
}

VulkanVariableBlockType VulkanVariableBlock::GetType() const {
    return _type;
}

uint32_t VulkanVariableBlock::GetOffset() const {
    return _offset;
}

uint32_t VulkanVariableBlock::GetSize() const {
    return _size;
}

std::string VulkanVariableBlock::GetName() const {
    return _name;
}

// ==== BlockMeta ==== //

VulkanVariableBlock& VulkanReflectedBlock::operator[](const std::string& name) {
    return _variables[name];
}

VulkanReflectedBlock& VulkanReflectedBlock::SetSize(uint32_t size)
{
    _size = size;
    return *this;
}

VulkanReflectedBlock& VulkanReflectedBlock::SetName(const std::string& name)
{
    _name = name;
    return *this;
}

std::vector<VulkanVariableBlock> VulkanReflectedBlock::GetMembers() const {
    std::vector<VulkanVariableBlock> out;
    out.reserve(_variables.size());

    for (const auto& pair : _variables) {
        out.push_back(pair.second);
    }

    return out;
}

uint32_t VulkanReflectedBlock::GetSize() const {
    return _size;
}

std::string VulkanReflectedBlock::GetName() const {
    return _name;
}

} // namespace bl