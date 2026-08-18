#pragma once

#include <nlohmann/json.hpp>

#include "Core/Object.h"
#include "Core/ReferenceCounted.h"
#include "ResourceID.h"

namespace bl {

class System;
class ResourceSystem;

enum class ResourceState {

    /** @brief The resource is in a prepared state where it can be used for operations. */
    eReady,

    /** @brief The resource has been released and can no longer be used for operations. */
    eReleased,

    /** @brief The resource is prepared for operations, may contain data that wasn't originally
                within the resource file. Can be potentially saved to file. */
    eReadyUnsaved
};

/**
 * @class Resource
 * @brief Base resource interface class for all resources.
 *
 * Resources are the building block for data loading and usage from multiple
 * sources in the engine. From models, textures, scene data, even user
 * generated content, resources keep the engine together.
 * Resource objects themselves should be kept relatively lightweight, they will
 * be instantiated throughout the program lifetime. Once the @ref Load function is
 * called, the heavy data lifting will begin.
 * 
 * On a default, a Resource will load/save/release it's 
 * 
 */
class Resource : public Object, public std::enable_shared_from_this<Resource> {
    OBJECT_BOILER_VIRTUAL(Resource, Object)

    std::filesystem::path _path; /** @brief Usually a path to the resource in the filesystem or name of the resource as described in the manifest, must be unique. */
    std::vector<std::shared_ptr<Resource>> _subResources; /** @brief Sub-resources that are part of this resource, but managed by it. */
    nlohmann::json _data;
    bool _loaded;

protected:
    friend class ResourceSystem;
    void SetJson(const nlohmann::json& data) { _data = data; }
    void SetPath(const std::filesystem::path& path) { _path = path; }
    void SetLoaded(bool isLoaded) { _loaded = isLoaded; }

public:

    /**
     * Default Constructor
     * 
     * A resource that does not exist in the filesystem. A filepath can be set
     * by calling SetPath().
     * 
     */
    Resource();

    /**
     * @brief Constructs a new Resource object.
     * @param manager Pointer to the resource manager that manages this resource.
     * @param data JSON data describing the resource.
     */
    Resource(const std::filesystem::path& path);

    /**
     * @brief Destructor for the Resource class.
     */
    virtual ~Resource() = 0;

    /**
     * @brief Performs resource tasks to prepare it for use. After this function finishes, the resource must be ready.
     */
    virtual void Load();

    /** 
     * @brief Returns true if the resource is in a loaded state.
     */
    bool IsLoaded() { return _loaded; }

    /**
     * @brief Release any resource that might need to be released before the engine shuts down.
     */
    virtual void Release();

    /**
     * @brief Save resources back to their file if they've been changed.
     */
    virtual void Save();

    /**
     * @brief Returns the current state of the resource.
     *      Resources are not always prepared for operations so checking
     *      the state is required every now and then.
     */
    ResourceState GetState() const;

    const nlohmann::json& GetJson() const { return _data; };

    /**
     * @brief Returns true if the resource is ready to be used.
     */
    bool IsReady() const;

    /**
     * @brief Returns the unique path of this resource.
     * @return The unique path of the resource.
     */
    const std::filesystem::path& GetPath();

    static void RegisterClass();

    void AddSubResource(std::shared_ptr<Resource> res);
};

} // namespace bl
