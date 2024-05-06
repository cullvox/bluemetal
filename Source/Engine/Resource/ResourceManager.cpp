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
    
    std::ifstream file(manifest);
    json root = json::parse(file);
    json resources = root["Resources"];

    for (const auto& data : resources)
    {
        auto path = data["Path"].get<std::string>();
        auto type = data["Type"].get<std::string>();

        auto builder = _builders.find(type);
        if (builder == _builders.end())
        {
            throw std::runtime_error("Could not find a builder!");
        }

        auto resource = builder->second->BuildResource(type, path, data);
        resource->SetPath(path);
        resource->SetLoadOp(ResourceLoadOp::eFile);
        resource->SetState(ResourceState::eUnloaded);

        _resources[path] = std::move(resource);
    }
}

void ResourceManager::UnloadUnreferenced()
{

}

} // namespace bl