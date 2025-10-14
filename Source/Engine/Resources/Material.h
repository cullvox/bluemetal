#pragma once

#include "MaterialInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterial.h"

namespace bl {

class Material : public MaterialInstance {
public:

    VulkanMaterial* GetMaterial() { return _material.get(); }
    const VulkanPipeline* GetPipeline();

private:
    Renderer* _renderer;
    VulkanDevice* _device;
    std::unique_ptr<VulkanMaterial> _material;
};

}