#include "Core/Print.h"
#include "ResourceSystem.h"

namespace bl 
{

ResourceSystem::ResourceSystem(Engine& engine)
    : System(engine)
{
}

ResourceSystem::~ResourceSystem()
{
}

std::unique_ptr<Resource> ResourceSystem::ConstructResource(ResourceSystem* resourceSystem, std::size_t typeHash, const std::filesystem::path& path)
{
    throw std::runtime_error("ResourceSystem::ConstructResource does not do anything.");
}


void ResourceSystem::UnloadUnreferenced()
{
}

} // namespace bl