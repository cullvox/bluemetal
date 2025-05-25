#pragma once

#include "Resource/Resource.h"
#include "VulkanMaterialInstance.h"
#include "Material.h"

namespace bl
{

class MaterialInstance : public Resource
{
public:
    MaterialInstance(const nlohmann::json& data, Material* material);
    ~MaterialInstance();

    virtual void Load() = 0;
    virtual void Unload() = 0;

private:
    ResourceRef<Material> material;
};

}