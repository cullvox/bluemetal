#pragma once

#include "Core/System.h"
#include "Precompiled.h"
#include "Resource.h"

#include <nlohmann/json.hpp>

namespace bl
{

class ResourceSystem : public System
{
    std::unordered_map<std::filesystem::path, std::unique_ptr<Resource>> _resources;
    std::unordered_map<std::size_t, System*> _resourceTypes; /** @brief Maps resource typeid(T).hash_code() to the system that handles it. */

public:
    ResourceSystem(Engine& engine);
    ~ResourceSystem();
    virtual std::unique_ptr<Resource> ConstructResource(ResourceSystem* resourceSystem, std::size_t typeHash, const std::filesystem::path& path) override;
    template<typename T> void AddSystemType(System* system);
    template<typename T> Ref<T> Load(const std::filesystem::path& path);
    template<typename T> Ref<T> Get(const std::filesystem::path& path);
    template<typename T> Ref<T> Add(const std::filesystem::path& path, T* resource); /** @brief Adds a resource to the manager assuming it's loaded. */
    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */
    void UnloadAll(); /** @brief Forcibly unloads all resources. */
};

template<typename TResource> void ResourceSystem::AddSystemType(System* system)
{
    static_assert(std::is_base_of<Resource, TResource>::value, "TResource must be a Resource type.");

    auto typeId = typeid(TResource).hash_code();
    if (_resourceTypes.find(typeId) != _resourceTypes.end()) {
        throw std::runtime_error("Could not register resource type as it already exists!");
    }

    _resourceTypes[typeId] = system;
}

template<typename T> Ref<T> ResourceSystem::Load(const std::filesystem::path& path)
{
    static_assert(std::is_base_of<Resource, T>::value, "T must be a Resource type.");

    auto systemIt = _resourceTypes.find(typeid(T).hash_code());
    if (systemIt == _resourceTypes.end()) {
        throw std::runtime_error("Could not load resource as no system is registered to handle it!");
    }
    System* system = systemIt->second;

    auto it = _resources.find(path);
    if (it == _resources.end())
    {
        try 
        {
            _resources[path] = system->ConstructResource(this, typeid(T).hash_code(), path);
            if (!_resources[path])
            {
                throw std::runtime_error("System failed to construct resource!");
            }
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
Ref<T> ResourceSystem::Get(const std::filesystem::path& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find a resource with path \"{}\"!", path);
        return Ref<T>{};
    }

    return ResourceRef{it->second.get()};
}

template<typename T>
Ref<T> ResourceSystem::Add(const std::filesystem::path& path, T* resource)
{
    auto it = _resources.find(path);
    if (it != _resources.end()) {
        throw std::runtime_error("Could not add a runtime resource as the path already exists!");
    }

    _resources[path] = std::unique_ptr<Resource>(resource);

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