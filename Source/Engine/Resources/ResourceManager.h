#pragma once

#include "Precompiled.h"
#include "Resource.h"

#include <nlohmann/json.hpp>

namespace bl {

template<typename T>
concept DerivedResource = std::is_base_of_v<Resource, T>; 

/**
 * @class ResourceManager
 */
class ResourceManager  {

    std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;

public:
    ResourceManager();
    ~ResourceManager();

    /**
     * @brief Creates a resource of type T with the specified path and data.
     * @param path The path to the resource.
     * @param data Any extra import data associated with this resource. This json data is
     *             stored within the manifest.
     * @return A reference to the newly created resource. Returns nullptr on failure.
     *
     * Any resource added via this function will be automatically added to the manifest when exported.
     */
    template <DerivedResource T>
    Ref<T> Load(const std::string& path);

    /**
     * @brief Retrieves a resource.
     * @param path The unique path of the resource to get.
     * 
     * This function will retrieve a resource regardless if it's data is loaded in memory or not.
     */
    template<DerivedResource T>
    Ref<T> Get(const std::string& path);
    bool Load(const std::string& path); /** @brief Ensures a resource is loaded. */
    void Add(const std::string_view& path, Resource* resource); /** @brief Adds a resource to the manager assuming it's loaded. */
    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */
};

template<DerivedResource T>
Ref<T> ResourceManager::Get(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find a resource with path \"{}\"!", path);
        return Ref<T>{};
    }

    return ResourceRef<T>{it->second.get()};
}

template<DerivedResource T>
Ref<T> ResourceManager::GetAndLoad(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find a resource with path \"{}\"!", path);
        return Ref<T>{};
    }

    return ResourceRef<T>{it->second.get()};
}

template <DerivedResource T>
Ref<T> ResourceManager::Add(const std::string& path, const nlohmann::json& data)
{

    auto it = _resources.find(path);
    if (it != _resources.end()) {
        throw std::runtime_error("Could not add a runtime resource as the path already exists!");
    }

    auto builder = _builders.find(type);
    if (builder == _builders.end()) {
        throw std::runtime_error("Could not find a builder!");
    }

    auto resource = builder->second->BuildResource(this, type, path, data);
    resource->SetPath(path);
    resource->SetLoadOp(ResourceLoadOp::eRuntime);
    resource->SetState(ResourceState::eUnloaded);

    _resources[path] = std::move(resource);

    resource->Load();

    return ResourceRef<T> { static_cast<T*>(resource.get()) };
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