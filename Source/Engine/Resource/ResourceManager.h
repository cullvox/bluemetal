#pragma once


#include "Precompiled.h"
#include "Resource.h"

#include <nlohmann/json.hpp>

namespace bl
{

class ResourceBuilder
{
public:
    ResourceBuilder() = default;
    virtual ~ResourceBuilder() = default;

    virtual std::unique_ptr<Resource> BuildResource(ResourceManager* manager, const std::string& type, const std::filesystem::path& path, const nlohmann::json& data) = 0;
};

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void RegisterBuilder(std::vector<std::string> types, ResourceBuilder* builder); /** @brief Registers a builder object to create resource references. */
    void LoadFromManifest(const std::filesystem::path& manifest);
    template<typename T> ResourceRef<T> Load(const std::string& path); /** @brief Loads any resource that isn't currently loaded into memory, just returns it if it already exists. */
    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */
    template<typename T> ResourceRef<T> AddRuntimeResource(const std::string& path, const std::string& type, const nlohmann::json& data);

private:
    std::multimap<std::string, ResourceBuilder*> _builders; /** @brief These builders build the resources inside the engine, some builders can build more than one type of resource hence a multimap. */
    std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;
};

template<typename T>
ResourceRef<T> ResourceManager::Load(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end())
    {
        throw std::runtime_error("Could not load an unavailable resource!");
    }

    auto& resource = it->second;
    resource->Load();

    return ResourceRef<T>{static_cast<T*>(resource.get())};
}

template<typename T>
ResourceRef<T> ResourceManager::AddRuntimeResource(const std::string& path, const std::string& type, const nlohmann::json& data)
{
    auto it = _resource.find(path);
    if (it != _resources.end())
    {
        throw std::runtime_error("Could not add a runtime resource as the path already exists!");
    }

    auto builder = _builders.find(type);
    if (builder == _builders.end())
    {
        throw std::runtime_error("Could not find a builder!");
    }

    auto resource = builder->second->BuildResource(this, type, path, data);
    resource->SetPath(path);
    resource->SetLoadOp(ResourceLoadOp::eRuntime);
    resource->SetState(ResourceState::eUnloaded);

    _resources[path] = std::move(resource);

    resource->Load();

    return ResourceRef<T>{static_cast<T*>(resource.get())};
}

} // namespace bl

/* How a resource manifest file works:

    It's a typical JSON file with data for each resource.

    Each resource itself has some data:
        type - string
        path - string       <- used as name in the resource manager 

    {
        "resources": [
            {
                "type": "sound"
                "path": "/some/path.wav"
            },
            {
                "type": "model",
                "path": "assets/models/fox.gltf"
            }
            {
                "type": "shader",
                "path": "assets/shaders/default_unlit.vert"
            }
        ]
    }

 */