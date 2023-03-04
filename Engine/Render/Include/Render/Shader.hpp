#pragma once
#include "spirv_reflect.h"
#pragma once

#include "Render/RenderDevice.hpp"

#include <spirv_reflect.h>

#include <vector>

namespace bl {
class Shader {
public:
    Shader(RenderDevice& renderDevice, const std::vector<uint32_t>& code);
    ~Shader();
    const VkShaderModule getShaderModule();
    std::vector<SpvReflectDescriptorSet> getDescriptorSets();
private:
    RenderDevice* m_pRenderDevice;
    VkShaderModule m_shaderModule;
    SpvReflectShaderModule m_reflectionModule;
};
} // namespace bl