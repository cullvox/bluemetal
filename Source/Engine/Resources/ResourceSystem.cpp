#include "ResourceSystem.h"
#include "Core/Print.h"
#include "Resources/Resource.h"
#include <algorithm>
#include <memory>
#include <unordered_map>

namespace bl {

ResourceSystem::ResourceSystem()
{
}

ResourceSystem::~ResourceSystem()
{
    ReleaseAll();
}

ResourceSystem* ResourceSystem::Get()
{
    static ResourceSystem system;
    return &system;
}

Ref<Resource> ResourceSystem::Load(const std::filesystem::path& path)
{

    // Check if the resource already exists.
    auto it = _resources.find(path);
    if (it != _resources.end()) {
        // Load the resource and we're done.
        if (!it->second->IsLoaded())
            it->second->Load();

        it->second->SetLoaded(true);
        return it->second;
    }

    std::ifstream file(path);
    nlohmann::json json;

    std::string classType;
    try {
        file >> json;
        classType = json["class"].get<std::string>();
    } catch (const std::exception& e) {
        
    }

    // Find a system to create the resource.
    const ClassData* data = ClassDB::Get()->FindClass(classType);

    if (!data) {
        throw std::runtime_error("Could not find a resource class in class database!");
    }

    if (!data->IsChildOf("Resource")) {
        throw std::runtime_error("Object type is not based on a resource!");
    }

    std::shared_ptr<Resource> res = std::shared_ptr<Resource>(data->Instantiate()->As<Resource>());
    res->SetJson(json);
    res->SetPath(path);

    // Create/load the resource since it doesn't exist.
    try {
        res->Load();
        res->SetLoaded(true);
        _resources[path] = res;
    } catch (const std::exception& e) {
        Print::Error("Could not load resource: {}, {}", path, e.what());
        return {};
    }

    return res;
}

void ResourceSystem::UnloadUnreferenced()
{
}

void ResourceSystem::ReleaseAll()
{
    for (auto& [name, resource] : _resources)
        resource->Release();

    _resources.clear();
}

} // namespace bl