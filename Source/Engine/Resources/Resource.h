#pragma once

#include <nlohmann/json.hpp>

#include "Core/Object.h"
#include "Core/ReferenceCounted.h"

namespace bl {

class System;
class ResourceSystem;

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
 */
class Resource : public Object, public std::enable_shared_from_this<Resource> {
    OBJECT_BOILER_VIRTUAL(Resource, Object)

    friend class ResourceSystem;
    std::filesystem::path _path; /** @brief Usually a path to the resource in the filesystem or name of the resource as described in the manifest, must be unique. */
    std::vector<std::shared_ptr<Resource>> _subResources; /** @brief Sub-resources that are part of this resource, but managed by it. */

public:
    /**
     * @brief Constructs a new Resource object.
     * @param manager Pointer to the resource manager that manages this resource.
     * @param data JSON data describing the resource.
     */
    Resource(Engine& engine, const std::filesystem::path& path);

    /**
     * @brief Destructor for the Resource class.
     */
    virtual ~Resource() = 0;

    /// Release any resource that might need to be released before the engine shuts down.
    virtual void Release();

    /**
     * @brief Returns the unique path of this resource.
     * @return The unique path of the resource.
     */
    const std::filesystem::path& GetPath();

    static void RegisterClass(ClassDB& db);

    void AddSubResource(std::shared_ptr<Resource> res);
};

} // namespace bl
