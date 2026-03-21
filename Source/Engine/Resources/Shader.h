#pragma once

#include "Graphics/GraphicsSystem.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanShader.h"
#include "Resource.h"

namespace bl {

class Shader : public Resource {
    VulkanDevice* _device;
    std::unique_ptr<VulkanShader> _shader;

public:
    Shader(Engine& engine, const std::filesystem::path& path);
    ~Shader();

    VulkanShader* Get() const { return _shader.get(); }
};

}