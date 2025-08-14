#pragma once

#include "Core/ReferenceCounted.h"
#include "Precompiled.h"

#include <nlohmann/json.hpp>

namespace bl {

class ResourceManager;

enum class ResourceSource {
    eFile,
    eBinary,
    eRuntime, /** @brief These resources are generated when the program begins, will not be saved in any way. */
    // eNetwork,
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
 */
class Resource : public ReferenceCounted {
    CLASS_OBJECT_VIRTUAL(Resource, ReferenceCounted)

    friend class ResourceManager;
    ResourceManager* _manager;
    std::string _path; /** @brief Usually a path to the resource in the filesystem or name of the resource as described in the manifest, must be unique. */
    ResourceSource _source; /** @brief Location of the resource, used for loading and saving. */
    nlohmann::json _importData; /** @brief Any specific import data associated with this resource. */
    std::size_t _packIndex; /** @brief Index for this resources storage pack path in the resource manager. */
    std::size_t _packByteOffset; /** @brief Byte offset to this resource in the pack. */
    std::size_t _packByteSize; /** @brief Size of the resource in the pack. */
    bool _isLoaded = false;

protected:

    /**
     * @brief Retrieves the manager that owns this resource.
     */
    ResourceManager* GetResourceManager() { return _manager; }

    /**
     * @brief Returns the storage location of this resource.
     * Resources can be stored in plenty of places including in the packed
     * binary format or just in a file. Depending on how or why the application
     * is packaged will determine how and where resources can be found.
     * We want to allow the potential for users to modify and add their own resources.
     */
    ResourceSource GetSource() const { return _source; }

    /**
     * @brief Returns the import data associated with this resource.
     * @returns This resources import data.
     * Import data is typically a small amount of metadata.
     */
    const nlohmann::json& GetImportData() const { return _importData; }

    /**
     * @brief Loads the resource from file.
     */
    virtual bool Load() = 0;

    /**
     * @brief Unloads the resource from memory.
     */
    virtual void Unload() = 0;

    /**
     * @brief Saves the resource to a binary format.
     * This function should be overridden in derived classes to implement binary saving logic.
     */
    virtual bool ExportBinary(std::ostream& out) const = 0;

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

    static void BindProperties() {} // Base resource class has no properties to bind.

    /**
     * @brief Returns the unique path of this resource.
     * @return The unique path of the resource.
     */
    const std::string& GetPath() const { return _path; }

    /**
     * @brief Returns the state of the resource.
     * @return The state of the resource.
     */
    bool IsLoaded() const { return _isLoaded; }
};

template <class T>
using Ref = ReferenceCounter<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl
