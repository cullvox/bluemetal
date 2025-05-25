#pragma once

#include "Resource/Resource.h"
#include "VulkanMaterial.h"
#include "MaterialInstance.h"

namespace bl
{

class Material : public Resource
{
public:
    Material(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device);
    ~Material();

    virtual void Load() override;
    virtual void Unload() override;
};

}