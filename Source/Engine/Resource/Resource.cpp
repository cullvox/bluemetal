#include "Resource.h"

namespace bl
{

Resource::Resource(ResourceManager* manager, const nlohmann::json&) 
    : _manager(manager)
{
}

Resource::~Resource() 
{
    if (_state == ResourceState::eLoaded)
        Unload();
}

const std::string& Resource::GetName() const
{
    return _name;
}

const std::filesystem::path& Resource::GetPath() const 
{
    return _path;
}

ResourceLoadOp Resource::GetLoadOp() const 
{
    return _loadOp;
}

ResourceState Resource::GetState() const 
{
    return _state;
}

void Resource::Load() 
{
    _state = ResourceState::eLoaded;
}

void Resource::Unload() 
{
    _state = ResourceState::eUnloaded;
}

void Resource::SetPath(const std::filesystem::path& path) 
{
    _path = path;
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