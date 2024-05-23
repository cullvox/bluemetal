#include "BlockReflection.h"

namespace bl
{

BlockMemberReflection& BlockReflection::operator[](const std::string& name)
{
    return _members[name];
}

std::vector<BlockMemberReflection> BlockReflection::GetMembers() const
{
    std::vector<BlockMemberReflection> out;
    out.reserve(_members.size());

    for (const auto& pair : _members)
        out.push_back(pair.second);

    return out;
}

void BlockReflection::SetBlockSize(uint32_t size)
{
    _blockSize = size;
}

uint32_t BlockReflection::GetBlockSize() const
{
    return _blockSize;
}


} // namespace bl