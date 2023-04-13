#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Shader.hpp"


blShader::blShader(const std::shared_ptr<blRenderDevice> renderDevice,
    std::span<uint32_t> code)
    : _renderDevice(renderDevice)
{
    const VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size_bytes(),
        .pCode = code.data()
    };

    if (vkCreateShaderModule(
            renderDevice->getDevice(), 
            &createInfo, 
            nullptr, 
            &_shaderModule) 
        != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan shader module!");
    }

    SpvReflectShaderModule reflectModule{};
    if (spvReflectCreateShaderModule(
            code.size_bytes(), 
            code.data(), 
            &reflectModule) 
        != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan reflection shader module!");
    }

    reflectModule.descriptor_sets->bindings[0];
}

blShader::~blShader() noexcept
{
    vkDestroyShaderModule(_renderDevice->getDevice(), _shaderModule, nullptr);
}
