#include "Resource.h"

namespace bl
{

std::string Resource::GetPath() const
{
    return _path;
}

uint64_t Resource::GetVersion() const
{
    return _version;
}

ResourceLoadOp Resource::GetLoadOp() const
{
    return _loadOp;
}

ResourceState Resource::GetState() const
{
    return _state;
}

void Resource::SetPath(const std::string& path)
{
    _path = path;
}

void Resource::SetVersion(uint64_t version)
{
    _version = version;
}

void Resource::SetLoadOp(ResourceLoadOp op)
{
    _loadOp = op;
}

void Resource::SetState(ResourceState state)
{
    _state = state;
}

} // namespace bl