#pragma once

#include "MaterialInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterial.h"

namespace bl {

class Material : public MaterialInstance {
public:
    virtual bool Load() override;
    virtual void Unload() override;
    virtual bool ExportBinary(std::ostream& stream) const override;

    VulkanMaterial* GetMaterial() { return _material.get(); }
    const VulkanPipeline* GetPipeline();

private:
    Renderer* _renderer;
    VulkanDevice* _device;
    std::unique_ptr<VulkanMaterial> _material;
};

}