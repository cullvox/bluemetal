#pragma once

#include "Precompiled.h"
#include "Resource.h"

#include <nlohmann/json.hpp>

namespace bl
{

class ResourceManager
{
    std::unordered_map<std::filesystem::path, std::unique_ptr<Resource>> _resources;

public:
    ResourceManager();
    ~ResourceManager();
    template<typename T> Ref<T> Load(const std::filesystem::path& path);
    template<typename T> Ref<T> Get(const std::filesystem::path& path);
    template<typename T> Ref<T> Add(const std::filesystem::path& path, T* resource); /** @brief Adds a resource to the manager assuming it's loaded. */
    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */
};

template<typename T> Ref<T> ResourceManager::Load(const std::filesystem::path& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {

        try 
        {
            _resources[path] = std::make_unique<T>(path);
        } 
        catch (...)
        {
            Print::Error("Could not load resource: {}", path);
            return Ref{(T*)nullptr};
        }

        return Ref{dynamic_cast<T*>(_resources[path].get())};
    }

    return Ref{dynamic_cast<T*>(it->second.get())};
}

template<typename T>
Ref<T> ResourceManager::Get(const std::filesystem::path& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find a resource with path \"{}\"!", path);
        return Ref<T>{};
    }

    return ResourceRef{it->second.get()};
}

template<typename T>
Ref<T> ResourceManager::Add(const std::filesystem::path& path, T* resource)
{
    auto it = _resources.find(path);
    if (it != _resources.end()) {
        throw std::runtime_error("Could not add a runtime resource as the path already exists!");
    }

    _resources[path] = std::make_unique<Resource>(resource);

    return Ref{_resources[path]};
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