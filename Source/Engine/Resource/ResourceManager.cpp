#include "ResourceManager.h"

namespace bl
{

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::RegisterBuilder(std::vector<std::string> types, ResourceBuilder* builder)
{
    for (const auto& type : types)
        _builders.emplace(type, builder);
}

void ResourceManager::LoadFromManifest(const std::filesystem::path& manifest)
{
    using namespace nlohmann;
    
    std::ifstream file(manifest, std::ios::binary | std::ios::in);
    if (file.bad())
        throw std::runtime_error("Could not open manifest file!");

    const json root = json::parse(file);
    json resources = root["resources"];

    for (const auto& data : resources)
    {
        auto path = data["relativePath"].get<std::string>();
        auto type = data["type"].get<std::string>();
        auto bakedPath = data["bakedPath"].get<std::string>();
        

        auto builder = _builders.find(type);
        if (builder == _builders.end())
        {
            throw std::runtime_error("Could not find a builder!");
        }

        auto resource = builder->second->BuildResource(this, type, path, data);
        resource->SetName(path);
        resource->SetPath(bakedPath);
        resource->SetLoadOp(ResourceLoadOp::eFile);
        resource->SetState(ResourceState::eUnloaded);

        _resources[path] = std::move(resource);
    }
}

void ResourceManager::UnloadUnreferenced()
{

}

} // namespace bl