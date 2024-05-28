#pragma once

#include "Precompiled.h"

namespace bl
{

enum class UniformMemberType
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

struct BlockMemberReflection
{
    std::string name;
    UniformMemberType type;
    uint32_t offset;
    uint32_t size;
};

class BlockReflection
{
public:
    BlockReflection() = default;
    ~BlockReflection() = default;

    BlockMemberReflection& operator[](const std::string& name);
    std::vector<BlockMemberReflection> GetMembers() const;
    void SetBlockSize(uint32_t size);
    uint32_t GetBlockSize() const;
    void SetBlockName(const std::string& name);
    std::string GetBlockName() const;

private:
    std::string _name;
    uint32_t _blockSize;
    std::unordered_map<std::string, BlockMemberReflection> _members;
};

} // namespace bl