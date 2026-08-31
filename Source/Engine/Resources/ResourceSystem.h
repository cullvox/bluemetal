#pragma once

#include "Core/Print.h"
#include "Core/ReferenceCounted.h"
#include "Core/Reference.h"
#include "Engine/System.h"
#include "Resource.h"
#include <memory>


namespace bl {

/**
 * @brief A manager of all engine resources, including models, sounds, textures, +more.
 */
class ResourceSystem : public System {
    std::unordered_map<std::filesystem::path, Ref<Resource>> _resources;
    std::unordered_map<std::size_t, System*> _resourceTypes; /** @brief Maps resource typeid(T).hash_code() to the system that handles it. */

    ResourceSystem();
    ~ResourceSystem();

public:
    static ResourceSystem* Get();

    Ref<Resource> Load(const std::filesystem::path& path);

    template <typename T>
    Ref<T> Load(const std::filesystem::path& path);

    template <typename T>
    Ref<T> Get(const std::filesystem::path& path);

    template <typename T>
    Ref<T> Add(const std::filesystem::path& path, Ref<T> resource); /** @brief Adds a resource to the manager assuming it's loaded. */

    template <typename T>
    Ref<T> AddSubResource(Ref<Resource> parent, Ref<T> resource); /** @brief Adds a sub-resource to a parent resource. */

    template <typename T>
    Ref<T> AddSubResource(Ref<Resource> parent); /** @brief Adds a sub-resource to a parent resource. */

    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */

    void ReleaseAll(); /** @brief Forcibly unloads all resources. */

    template <typename T>
    System* GetSystemType();
};

template <typename T>
Ref<T> ResourceSystem::Load(const std::filesystem::path& path)
{
    static_assert(std::is_base_of<Resource, T>::value, "T must be a Resource type.");
    return std::dynamic_pointer_cast<T>(Load(path).lock());
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
Ref<T> ResourceSystem::Add(const std::filesystem::path& path, Ref<T> resource)
{
    auto it = _resources.find(path);
    if (it != _resources.end()) {
        throw std::runtime_error("Could not add a runtime resource as the path already exists!");
    }

    _resources[path] = resource;

    return std::dynamic_pointer_cast<T>(_resources[path]);
}

template <typename T>
Ref<T> ResourceSystem::AddSubResource(Ref<Resource> parent, Ref<T> resource)
{
    parent->_subResources.push_back(std::move(resource));
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

} // namespace bl
