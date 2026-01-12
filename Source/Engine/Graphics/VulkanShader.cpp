#include "Core/Print.h"
#include "VulkanConversions.h"
#include "VulkanShader.h"

namespace bl 
{

VulkanShader::VulkanShader(VulkanDevice* device, std::span<const uint32_t> bytes)
    : _device(device)
    , _stage(VK_SHADER_STAGE_ALL)
    , _reflect()
    , _module(VK_NULL_HANDLE) 
{
    // Create the reflection module for pipeline usage.
    if (spvReflectCreateShaderModule(bytes.size(), bytes.data(), &_reflect) != SPV_REFLECT_RESULT_SUCCESS) 
    {
        throw std::runtime_error("Could not preform reflection on a shader module!");
    }

    _stage = static_cast<VkShaderStageFlagBits>(_reflect.shader_stage);

    // Create the shader module.
    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = bytes.size();
    moduleCreateInfo.pCode = bytes.data();

    VK_CHECK(vkCreateShaderModule(_device->Get(), &moduleCreateInfo, nullptr, &_module))
}

VulkanShader::~VulkanShader() 
{
    if (_module == VK_NULL_HANDLE) return;

    vkDestroyShaderModule(_device->Get(), _module, nullptr); 
    spvReflectDestroyShaderModule(&_reflect);

    _module = VK_NULL_HANDLE;
}

VkShaderStageFlagBits VulkanShader::GetStage() const 
{
    return _stage; 
}

const SpvReflectShaderModule& VulkanShader::GetReflection() const 
{
    return _reflect;
}

VkShaderModule VulkanShader::Get() const 
{
    return _module;
}

} // namespace bl
