#pragma once

#include "Precompiled.h"
#include "Resource.h"

#include <nlohmann/json.hpp>

namespace bl {

/**
 * @class ResourceBuilder
 */
class ResourceBuilder {
protected:
    virtual ~ResourceBuilder() = default;

    friend class ResourceManager;

    /** 
     * @brief Builds a resource of a specific type for the resource manager.
     */
    virtual std::unique_ptr<Resource> BuildResource(ResourceManager* manager, const std::string& type) = 0;

    /**
     * @brief Add all default resources--for types this builder creates--to the manager.
     */
    virtual void AddDefaultResources(ResourceManager* manager) = 0;
};

template<typename T>
concept DerivedResource = std::is_base_of_v<Resource, T>; 

/**
 * @class ResourceManager
 */
class ResourceManager  {

    std::unordered_map<std::string, ResourceBuilder*> _builders; /** @brief These builders build the resources inside the engine, some builders can build more than one type of resource hence a multimap. */
    std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;
    std::vector<std::string> _packPaths;

public:
    ResourceManager();
    ~ResourceManager();

    /**
     * @brief Registers a resource builder for a specific type of resource.
     * @param types A vector of strings representing the types this builder can handle.
     * @param builder A pointer to the ResourceBuilder instance that will handle the resource creation. Pointer will not be managed by the resource manager.
     *
     * In order for a resource to be properly built it's builder must already be registered.
     */
    void RegisterBuilder(const std::vector<std::string>& types, ResourceBuilder* builder);

    /**
     * @brief Builds resources from a manifest file.
     * @param manifest The path to the manifest file containing resource definitions.
     *
     * Loads a set of resources from a JSON manifest file.
     */
    bool BuildResourcesFromManifest(const std::string& manifestPath);

    /**
     * @brief Builds resources from a pack manifest.
     * @param packPath Path to load the pack from.
     *
     * Loads a set of resources from the pack file using the packs internal
     * CBOR encoded manifest. These files are not designed to be human readable
     * and are exported from the resource manager itself.
     *
     * This function can be ran as many times as the user would like. For every pack
     * loaded it's information will be stored for resource loading later. Loading resources
     * from a pack and from a manifest file @ref BuildFromManifest can be all loaded together
     * in this same resource manager, feel free to use both.
     */
    bool BuildResourcesFromPack(const std::string& packPath);

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
    Ref<T> Add(const std::string& path, const nlohmann::json& importData = {});

    /**
     * @brief Retrieves a resource.
     * @param path The unique path of the resource to get.
     * 
     * This function will retrieve a resource regardless if it's data is loaded in memory or not.
     */
    template<DerivedResource T>
    Ref<T> Get(const std::string& path);

    /**
     * @brief Loads a resource by its path.
     * @param path The path to the resource to be loaded.
     * @return A reference to the loaded resource.
     */
    template <DerivedResource T>
    Ref<T> GetAndLoad(const std::string& path);

    /**
     * @brief Removes the resources from the manager.
     * @param path The path of the resource to be deleted.
     * 
     * If this program is restarted and it is still within either a binary or 
     * json manifest the unlisted resource will reappear. Save the manifest or reexport
     * the package to ensure the resource is removed.
     */
    bool Unlist(const std::string& path);

    /**
     * @brief Loads a single resource.
     */
    bool Load(const std::string& path);

    /**
     * @brief Loads many resources at once.
     */
    bool LoadGroup(const std::vector<std::string>& paths);

    /**
     * @brief 
     */
    bool GetResourcePack(Resource* resource, std::ifstream& outFile, std::size_t& outByteSize);

    void UnloadUnreferenced(); /** @brief Cleans up memory by unloading resources that aren't currently needed. Abides by a ResourceLoadOp. */

    /* ========== PACKAGING/MANIFEST ========== */

    /**
     * @brief Returns all resource paths.
     * @return A vector containing all resource paths.
     *
     * Use this function when you'd like to retrieve a list of all resource paths
     * for selecting when exporting.
     */
    std::vector<std::string> GetAllResourcePaths() const;

    /**
     * @brief Exports a list of resources to a manifest file.
     */
    void ExportManifest(const std::string& exportedManifestPath);
    void ExportManifest(const std::string& exportedManifestPath, const std::vector<std::string>& resourcesToExport);
    void ExportPackage(const std::string& exportedPackPath);
    void ExportPackage(const std::string& exportedPackPath, const std::vector<std::string>& resourcesToExport);
};

template<DerivedResource T>
Ref<T> ResourceManager::Get(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Log::Error("Could not find a resource with path \"{}\"!", path);
        return Ref<T>{};
    }

    return ResourceRef<T>{it->second.get()};
}

template<DerivedResource T>
Ref<T> ResourceManager::GetAndLoad(const std::string& path)
{
    auto it = _resources.find(path);
    if (it == _resources.end()) {
        Log::Error("Could not find a resource with path \"{}\"!", path);
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

template <typename... TResourceTypes>
void RegisterBuilder(std::unique_ptr<ResourceBuilder> builder)
{
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