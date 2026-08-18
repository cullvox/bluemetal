#pragma once

#include "Core/Object.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanShader.h"
#include "Resource.h"

namespace bl {

class Shader : public Resource {
    OBJECT_BOILER(Shader, Resource)

    VulkanDevice* _device;
    std::unique_ptr<VulkanShader> _shader;

public:
    Shader();
    Shader(const std::filesystem::path& path);
    Shader(const Shader& shader);
    ~Shader();

    virtual void Load() override;
    virtual void Release() override;
    static void RegisterClass(); 

    VulkanShader* Get() const { return _shader.get(); }
};

}