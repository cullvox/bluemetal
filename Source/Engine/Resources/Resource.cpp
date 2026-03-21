#include "Resource.h"
#include "Core/Print.h"

namespace bl {

Resource::Resource(Engine& engine, const std::filesystem::path& path)
    : Object(engine)
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

void Resource::AddSubResource(std::shared_ptr<Resource> res)
{
    _subResources.push_back(res);
}

} // namespace bl