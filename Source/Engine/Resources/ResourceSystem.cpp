#include "ResourceSystem.h"
#include "Core/Print.h"

namespace bl {

ResourceSystem::ResourceSystem(Engine& engine)
    : System(engine)
{
}

ResourceSystem::~ResourceSystem()
{
    ReleaseAll();
}

std::shared_ptr<Resource> ResourceSystem::ConstructResource(std::size_t, const std::filesystem::path&)
{
    throw std::runtime_error("ResourceSystem::ConstructResource does not do anything.");
}

void ResourceSystem::UnloadUnreferenced()
{
}

void ResourceSystem::ReleaseAll()
{
    for (auto& [name, resource] : _resources)
        resource->Release();
}

} // namespace bl