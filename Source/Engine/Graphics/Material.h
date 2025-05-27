#pragma once

#include "Resource/Resource.h"
#include "Renderer.h"
#include "VulkanMaterial.h"

namespace bl
{

class Material : public Resource
{
public:
    Material(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device, Renderer* renderer);
    ~Material();

    virtual void Load() override;
    virtual void Unload() override;
private:
    std::unique_ptr<VulkanMaterial> _material;
};

}