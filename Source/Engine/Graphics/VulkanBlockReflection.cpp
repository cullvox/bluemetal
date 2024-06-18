#include "VulkanBlockReflection.h"

namespace bl {

VulkanBlockVariable& VulkanBlockVariable::SetBinding(uint32_t binding) {
    _binding = binding;
    return *this;
}

VulkanBlockVariable& VulkanBlockVariable::SetType(VulkanBlockVariableType type) {
    _type = type;
    return *this;
}

VulkanBlockVariable& VulkanBlockVariable::SetOffset(uint32_t offset) {
    _offset = offset;
    return *this;
}

VulkanBlockVariable& VulkanBlockVariable::SetSize(uint32_t size) {
    _size = size;
    return *this;
}

VulkanBlockVariable& VulkanBlockVariable::SetName(const std::string& name) {
    _name = name;
    return *this;
}

uint32_t VulkanBlockVariable::GetBinding() const {
    return _binding;
}

VulkanBlockVariableType VulkanBlockVariable::GetType() const {
    return _type;
}

uint32_t VulkanBlockVariable::GetOffset() const {
    return _offset;
}

uint32_t VulkanBlockVariable::GetSize() const {
    return _size;
}

std::string VulkanBlockVariable::GetName() const {
    return _name;
}

// ==== BlockMeta ==== //

VulkanBlockVariable& VulkanBlockReflection::operator[](const std::string& name) {
    return _variables[name];
}

VulkanBlockReflection& VulkanBlockReflection::SetSize(uint32_t size)
{
    _size = size;
    return *this;
}

VulkanBlockReflection& VulkanBlockReflection::SetName(const std::string& name)
{
    _name = name;
    return *this;
}

std::vector<VulkanBlockVariable> VulkanBlockReflection::GetMembers() const {
    std::vector<VulkanBlockVariable> out;
    out.reserve(_variables.size());

    for (const auto& pair : _variables) {
        out.push_back(pair.second);
    }

    return out;
}

uint32_t VulkanBlockReflection::GetSize() const {
    return _size;
}

std::string VulkanBlockReflection::GetName() const {
    return _name;
}

} // namespace bl