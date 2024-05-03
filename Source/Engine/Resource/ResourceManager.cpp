#include "ResourceManager.h"

namespace bl
{

ResourceManager();
~ResourceManager();

void ResourceManager::RegisterBuilder(std::vector<std::string> types, std::shared_ptr<ResourceBuilder> builder)
{

}

void ResourceManager::LoadFromManifest(const std::filesystem::path& manifest)
{

}

template<typename T> ResourceRef<T> ResourceManager::Load(const std::string& name)
{

}

void ResourceManager::UnloadUnreferenced()
{

}

} // namespace bl