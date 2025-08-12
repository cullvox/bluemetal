#pragma once

#include "Graphics/VulkanShader.h"
#include "Resource.h"

namespace bl {

class Shader : public Resource {
    VulkanDevice* _device;
    std::unique_ptr<VulkanShader> _shader;

public:
    virtual const std::string& GetType() const override { return "Shader"; }

    Shader(ResourceManager* manager, VulkanDevice* device);
    ~Shader();

    virtual bool Load() override;
    virtual void Unload() override;
    virtual bool ExportBinary(std::ostream& stream) const override;
};

}