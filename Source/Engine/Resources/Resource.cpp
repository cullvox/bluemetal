#include "Resource.h"
#include "Core/Print.h"

namespace bl {

Resource::Resource(ResourceSystem* resourceSystem, System*, const std::filesystem::path& path)
    : _resourceSystem(resourceSystem)
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

void Resource::AddSubResource(std::shared_ptr<Resource> res)
{
    _subResources.push_back(res);
}

} // namespace bl