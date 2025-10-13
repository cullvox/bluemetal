#include "Resource.h"
#include "Core/Print.h"

namespace bl {

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