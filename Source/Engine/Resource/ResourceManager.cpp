#include "ResourceManager.h"

namespace bl
{

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::RegisterBuilder(std::vector<std::string> types, std::shared_ptr<ResourceBuilder> builder)
{

}

void ResourceManager::LoadFromManifest(const std::filesystem::path& manifest)
{

}

void ResourceManager::UnloadUnreferenced()
{

}

} // namespace bl