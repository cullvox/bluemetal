#include "Resource.h"
#include "Core/Print.h"

namespace bl {

Resource::Resource(ResourceSystem* resourceSystem, System*, const std::filesystem::path& path)
    : ReferenceCounted()
    , _resourceSystem(resourceSystem)
    , _path(path)
{
}

Resource::~Resource()
{
}

} // namespace bl