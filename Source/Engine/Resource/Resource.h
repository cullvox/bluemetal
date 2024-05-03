#pragma once

#include "Precompiled.h"
#include "Core/ReferenceCounted.h"
#include "Core/Json.h"

namespace bl 
{

enum class ResourceLoadOp
{
    eFile, /** @brief This resource can be loaded in and out at runtime as needed. */
    eRuntime, /** @brief Will delete the resource after it's unloaded. */
    eNoUnload, /** @brief Will not unload even if there are no references. */
};

enum class ResourceState
{
    eLoaded,
    eUnloaded,
};

class Resource : public ReferenceCounted
{
public:
    Resource() = default;
    virtual ~Resource() = default;

    std::string GetPath() const;
    uint64_t GetVersion() const;
    ResourceLoadOp GetLoadOp() const;
    ResourceState GetState() const;

    virtual void Load(const nlohmann::json& data) = 0;
    virtual void Unload() = 0;

protected:
    friend class ResourceManager;

    void SetPath(const std::string& path);
    void SetVersion(uint64_t version);
    void SetLoadOp(ResourceLoadOp op);
    void SetState(ResourceState state);

private:
    std::string _path; /** @brief Usually a path to the resource in the filesystem. Name of the resource as described in the manifest, must be unique. */
    uint64_t _version; /** @brief Version of this resource, can change between versions of end software with patches. */
    ResourceLoadOp _loadOp;
    ResourceState _state;
};

template<typename T>
using ResourceRef = ReferenceCounter<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl
