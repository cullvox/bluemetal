#include "Resource.h"
#include "Core/ClassDB.h"
#include "Core/Print.h"
#include "Engine/Engine.h"

namespace bl {

Resource::Resource()
    : Object()
    , _loaded(false)
{
}

Resource::Resource(const std::filesystem::path& path)
    : Object()
    , _path(path)
{
}

Resource::~Resource()
{
    // Delete all subresources
    for (auto subResource : _subResources) {
        subResource.reset();
    }
}

const std::filesystem::path& Resource::GetPath()
{
    return _path;
}

void Resource::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterVirtualClass("Resource", "Object");
}

void Resource::Load()
{
}

void Resource::Release()
{
    SetLoaded(false);
}

void Resource::Save()
{
}

ResourceState Resource::GetState() const
{
    return ResourceState::eReady;
}

bool Resource::IsReady() const
{
    auto state = GetState(); 
    return state == ResourceState::eReady || state == ResourceState::eReadyUnsaved; 
}

void Resource::AddSubResource(std::shared_ptr<Resource> res)
{
    _subResources.push_back(res);
}

} // namespace bl