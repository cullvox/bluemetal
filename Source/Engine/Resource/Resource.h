#pragma once

#include "Precompiled.h"
#include "Core/ReferenceCounted.h"

#include <nlohmann/json.hpp>

namespace bl 
{

enum class ResourceLoadOp {
    eFile, /** @brief This resource can be loaded in and out at runtime as needed. */
    eRuntime, /** @brief Will delete the resource after it's unloaded. */
    eNoUnload, /** @brief Will not unload even if there are no references. */
};

enum class ResourceState {
    eLoaded,
    eUnloaded,
};

class Resource : public ReferenceCounted {
public:
    Resource(const nlohmann::json& data);
    virtual ~Resource();

    std::filesystem::path GetPath() const;
    uint64_t GetVersion() const;
    ResourceLoadOp GetLoadOp() const;
    ResourceState GetState() const;

    virtual void Load();
    virtual void Unload();

protected:
    friend class ResourceManager;

    void SetPath(const std::filesystem::path& path);
    void SetVersion(uint64_t version);
    void SetLoadOp(ResourceLoadOp op);
    void SetState(ResourceState state);

private:
    std::filesystem::path _path; /** @brief Usually a path to the resource in the filesystem. Name of the resource as described in the manifest, must be unique. */
    uint64_t _version; /** @brief Version of this resource, can change between versions of end software with patches. */
    ResourceLoadOp _loadOp;
    ResourceState _state;
};

template<class T>
using ResourceRef = ReferenceCounter<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl
