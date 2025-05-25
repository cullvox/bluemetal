#pragma once

#include "Precompiled.h"
#include "Core/ReferenceCounted.h"

#include <nlohmann/json.hpp>

namespace bl 
{

class ResourceManager;

enum class ResourceLoadOp 
{
    eFile, /** @brief This resource can be loaded in and out at runtime as needed. */
    eRuntime, /** @brief Will delete the resource after it's unloaded. */
    ePersistent, /** @brief Will not unload automatically unless deleted. */
};

enum class ResourceState 
{
    eLoaded,
    eUnloaded,
};

class Resource : public ReferenceCounted 
{
public:
    Resource(ResourceManager* manager, const nlohmann::json& data);
    virtual ~Resource();

    const std::string& GetName() const;
    const std::filesystem::path& GetPath() const;
    ResourceLoadOp GetLoadOp() const;
    ResourceState GetState() const;

    virtual void Load() = 0;
    virtual void Unload() = 0;

protected:
    friend class ResourceManager;

    void SetName(const std::string& name);
    void SetPath(const std::filesystem::path& path);
    void SetLoadOp(ResourceLoadOp op);
    void SetState(ResourceState state);

private:
    ResourceManager* _manager;
    std::string _name;
    std::filesystem::path _path; /** @brief Usually a path to the resource in the filesystem. Name of the resource as described in the manifest, must be unique. */
    ResourceLoadOp _loadOp;
    ResourceState _state;
};

template<class T>
using ResourceRef = ReferenceCounter<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl
