#pragma once

#include "Core/ReferenceCounted.h"

namespace bl 
{

enum class ResourceLoadOp
{
    eFile, /** @brief This resource can be loaded in and out at runtime as needed. */
    eNoUnload, /** @brief Will not unload even if there are no references. */
};

class Resource : public ReferenceCounted
{
public:
    Resource() = default;
    ~Resource() = default;

    std::string GetName();
    uint64_t GetVersion();
    ResourceLoadOp GetLoadOp();
    
    virtual void Load() = 0;
    virtual void Unload() = 0;

private:
    std::string _name; /** @brief Name of the resource as described in the manifest, is unique. */
    uint64_t _version; /** @brief Version of this resource, can change between versions of end software with patches. */
    ResourceLoadOp _loadOp;
};

template<typename T>
class ResourceReference
{
public:
    static_assert(std::is_base_of_v<Resource, T>);

    ResourceReference(T& resource);
    ~ResourceRefrence();

private:


    T& _resource;
}

}
