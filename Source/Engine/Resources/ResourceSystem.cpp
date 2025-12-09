#include "ResourceSystem.h"
#include "Core/Print.h"

namespace bl {

ResourceSystem::ResourceSystem(Engine& engine)
    : System(engine)
{
}

ResourceSystem::~ResourceSystem()
{
    UnloadAll();
}

std::shared_ptr<Resource> ResourceSystem::ConstructResource(ResourceSystem&, std::size_t, const std::filesystem::path&)
{
    throw std::runtime_error("ResourceSystem::ConstructResource does not do anything.");
}

void ResourceSystem::UnloadUnreferenced()
{
}

void ResourceSystem::UnloadAll()
{
    for (auto& [name, resource] : _resources)
        resource.reset();
}

} // namespace bl