#include "Resource.h"
#include "Core/Print.h"

namespace bl {

OBJECT_REGISTRATION(Resource)

Resource::Resource()
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