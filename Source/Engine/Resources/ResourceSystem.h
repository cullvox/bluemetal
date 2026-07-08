#pragma once

#include "Core/Print.h"
#include "Core/ReferenceCounted.h"
#include "Engine/System.h"
#include "Resource.h"


namespace bl {

/**
 * @brief A manager of all engine resources, including models, sounds, textures, +more.
 */
class ResourceSystem : public System {
    std::unordered_map<std::filesystem::path, std::shared_ptr<Resource>> _resources;
    std::unordered_map<std::size_t, System*> _resourceTypes; /** @brief Maps resource typeid(T).hash_code() to the system that handles it. */

public:
    ResourceSystem(Engine& engine);
    ~ResourceSystem();
    virtual std::shared_ptr<Resource> ConstructResource(std::size_t typeHash, const std::filesystem::path& path) override;
    template <typename T>
    void AddSystemType(System* system);
    template <typename T>
    Ref<T> Load(const std::filesystem::path& path);
    template <typename T>
    Ref<T> Get(const std::filesystem::path& path);
    template <typename T>
    Ref<T> Add(const std::filesystem::path& path, std::shared_ptr<T> resource); /** @brief Adds a resource to the manager assuming it's loaded. */
    template <typename T>
    Ref<T> AddSubResource(Ref<Resource> parent, std::shared_ptr<T> resource); /** @brief Adds a sub-resource to a parent resource. */
    template <typename T>
    Ref<T> AddSubResource(Ref<Resource> parent); /** @brief Adds a sub-resource to a parent resource. */
    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */
    void ReleaseAll(); /** @brief Forcibly unloads all resources. */
    template <typename T>
    System* GetSystemType();
};

template <typename TResource>
void ResourceSystem::AddSystemType(System* system)
{
    static_assert(std::is_base_of<Resource, TResource>::value, "TResource must be a Resource type.");

    auto typeId = typeid(TResource).hash_code();
    if (_resourceTypes.find(typeId) != _resourceTypes.end()) {
        throw std::runtime_error("Could not register resource type as it already exists!");
    }

    _resourceTypes[typeId] = system;
}

template <typename T>
Ref<T> ResourceSystem::Load(const std::filesystem::path& path)
{
    static_assert(std::is_base_of<Resource, T>::value, "T must be a Resource type.");

    // Find a system to create the resource.
    auto systemIt = _resourceTypes.find(typeid(T).hash_code());
    if (systemIt == _resourceTypes.end()) {
        throw std::runtime_error("Could not load resource as no system is registered to handle it!");
    }
    System* system = systemIt->second;

    // Look if the resource exists, create it if it doesn't exist.
    auto it = _resources.find(path);
    if (it != _resources.end()) {

        // These dynamic casts when returning should never fail.
        // If by some miracle it does, the ConstructResource function
        // in the system created the wrong type.
        if (auto res = std::dynamic_pointer_cast<T>(it->second)) {
            return res;
        } else {
            throw std::runtime_error("system->ConstructResource() failed and created the wrong type.");
        }
    }

    // Create/load the resource since it doesn't exist.
    try {
        _resources[path] = system->ConstructResource(typeid(T).hash_code(), path);
        if (!_resources[path]) {
            throw std::runtime_error("System failed to construct resource!");
        }
    } catch (const std::exception& e) {
        Print::Error("Could not load resource: {}", path);
        Print::Error("{}", e.what());
        return {};
    }

    if (auto res = std::dynamic_pointer_cast<T>(_resources[path])) {
        return res;
    } else {
        throw std::runtime_error("system->ConstructResource() failed and created the wrong type.");
    }
}

template <typename T>
Ref<T> ResourceSystem::Get(const std::filesystem::path& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Print::Error("Could not find a resource with path \"{}\"!", path);
        return Ref<T> {};
    }

    return it->second;
}

template <typename T>
Ref<T> ResourceSystem::Add(const std::filesystem::path& path, std::shared_ptr<T> resource)
{
    auto it = _resources.find(path);
    if (it != _resources.end()) {
        throw std::runtime_error("Could not add a runtime resource as the path already exists!");
    }

    _resources[path] = resource;

    return std::dynamic_pointer_cast<T>(_resources[path]);
}

template <typename T>
Ref<T> ResourceSystem::AddSubResource(Ref<Resource> parent, std::shared_ptr<T> resource)
{
    parent.lock()->_subResources.push_back(std::move(resource));
    return resource;
}

template <typename T>
Ref<T> ResourceSystem::AddSubResource(Ref<Resource> parent)
{
    System* system = GetSystemType<T>();
    std::shared_ptr<Resource> resource = system->ConstructResource(typeid(T).hash_code(), ""); // Sub-resources do not have paths.

    if (auto res = std::dynamic_pointer_cast<T>(resource)) {
        return AddSubResource<T>(parent, std::move(res));
    } else {
        throw std::runtime_error("system->ConstructResource() failed and created the wrong type.");
    }
}

template <typename T>
System* ResourceSystem::GetSystemType()
{
    auto systemIt = _resourceTypes.find(typeid(T).hash_code());
    if (systemIt == _resourceTypes.end()) {
        throw std::runtime_error("Could not get system as no system is registered to handle it!");
    }
    return systemIt->second;
}

} // namespace bl
