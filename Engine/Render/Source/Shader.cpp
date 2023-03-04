#include "Render/Shader.hpp"
#include "spirv_reflect.h"

namespace bl {

Shader::Shader(RenderDevice& renderDevice, const std::vector<uint32_t>& code)
    : m_pRenderDevice(&renderDevice)
{

    // Reflect the shader for its data.
    if (spvReflectCreateShaderModule(code.size(), code.data(), &m_reflectionModule) != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not reflect a SPIR-V shader module!");
    }

    // Create the shader module
    const VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = (uint32_t)code.size(),
        .pCode = code.data()
    };

    if (vkCreateShaderModule(m_pRenderDevice->getDevice(), &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
    {
        spvReflectDestroyShaderModule(&m_reflectionModule);
        throw std::runtime_error("Could not create a vulkan shader module!");
    }
}

Shader::~Shader()
{
    spvReflectDestroyShaderModule(&m_reflectionModule);
    vkDestroyShaderModule(m_pRenderDevice->getDevice(), m_shaderModule, nullptr);
}


} // namespace bl