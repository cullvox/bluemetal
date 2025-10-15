#pragma once

#include "Core/ReferenceCounted.h"
#include "Precompiled.h"

#include <nlohmann/json.hpp>

namespace bl {

class ResourceManager;

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
class Resource : public ReferenceCounted 
{
    friend class ResourceManager;
    std::string _path; /** @brief Usually a path to the resource in the filesystem or name of the resource as described in the manifest, must be unique. */

public:
    /**
     * @brief Constructs a new Resource object.
     * @param manager Pointer to the resource manager that manages this resource.
     * @param data JSON data describing the resource.
     */
    Resource();

    /**
     * @brief Destructor for the Resource class.
     */
    virtual ~Resource() = 0;

    /**
     * @brief Returns the unique path of this resource.
     * @return The unique path of the resource.
     */
    const std::string& GetPath() const { return _path; }
};

} // namespace bl
