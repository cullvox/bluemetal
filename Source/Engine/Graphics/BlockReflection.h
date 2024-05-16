#pragma once

#include "Precompiled.h"

namespace bl
{

enum class BlockMemberType
{
    eScalarBool,
    eScalarInt,
    eScalarFloat,
    eVector2b,
    eVector2i,
    eVector2f,
    eVector3b,
    eVector3i,
    eVector3f,
    eVector4b,
    eVector4i,
    eVector4f,
    eMatrix2f,
    eMatrix3f,
    eMatrix4f,
};

struct BlockMember
{
    std::string name;
    BlockMemberType type;
    uint32_t offset;
    uint32_t size;
};

class BlockMembersBuilder
{
public:
    BlockMembersBuilder() = default;
    ~BlockMembersBuilder() = default;

    BlockMember& operator[](const std::string& name); // Shorthanded for AccessOrInsertMember().
    std::vector<BlockMember> GetMembers() const;
    std::unordered_map<std::string, BlockMember>& GetMembersMap();

private:
    std::unordered_map<std::string, BlockMember> _members;
}

} // namespace bl