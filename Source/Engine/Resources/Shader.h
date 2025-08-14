#pragma once

#include "Graphics/VulkanShader.h"
#include "Resource.h"

namespace bl {

class Shader : public Resource {
    CLASS_OBJECT(Shader, Resource)

    VulkanDevice* _device;
    std::unique_ptr<VulkanShader> _shader;

public:
    Shader();
    ~Shader();

    virtual bool Load() override;
    virtual void Unload() override;
    virtual bool ExportBinary(std::ostream& stream) const override;
};

}