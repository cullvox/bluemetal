#include "BlockMeta.h"

namespace bl {

BlockVariable& BlockVariable::SetBinding(uint32_t binding) {
    _binding = binding;
    return *this;
}

BlockVariable& BlockVariable::SetType(BlockVariableType type) {
    _type = type;
    return *this;
}

BlockVariable& BlockVariable::SetOffset(uint32_t offset) {
    _offset = offset;
    return *this;
}

BlockVariable& BlockVariable::SetSize(uint32_t size) {
    _size = size;
    return *this;
}

BlockVariable& BlockVariable::SetName(const std::string& name) {
    _name = name;
    return *this;
}

uint32_t BlockVariable::GetBinding() const {
    return _binding;
}

BlockVariableType BlockVariable::GetType() const {
    return _type;
}

uint32_t BlockVariable::GetOffset() const {
    return _offset;
}

uint32_t BlockVariable::GetSize() const {
    return _size;
}

std::string BlockVariable::GetName() const {
    return _name;
}

// ==== BlockMeta ==== //

BlockVariable& BlockMeta::operator[](const std::string& name) {
    return _variables[name];
}

BlockMeta& BlockMeta::SetSize(uint32_t size)
{
    _size = size;
    return *this;
}

BlockMeta& BlockMeta::SetName(const std::string& name)
{
    _name = name;
    return *this;
}

std::vector<BlockVariable> BlockMeta::GetMembers() const {
    std::vector<BlockVariable> out;
    out.reserve(_variables.size());

    for (const auto& pair : _variables) {
        out.push_back(pair.second);
    }

    return out;
}

uint32_t BlockMeta::GetSize() const {
    return _size;
}

std::string BlockMeta::GetName() const {
    return _name;
}

} // namespace bl