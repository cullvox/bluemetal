#include "Resource.h"
#include "Core/Print.h"

namespace bl {

Resource::Resource(ResourceManager* manager)
    : _manager(manager)
{
}

bool Resource::Load()
{
    _isLoaded = true;
    return true;
}

void Resource::Unload()
{
    _isLoaded = false;
}

} // namespace bl