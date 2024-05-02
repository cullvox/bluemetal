#include "Shader.h"
#include "Conversions.h"
#include "Core/Print.h"

namespace bl {

Shader::Shader(Device* device, VkShaderStageFlagBits stage, const std::vector<char>& binary)
    : _device(device)
    , _stage(stage)
    , _reflect()
    , _module(VK_NULL_HANDLE)
{

    if (spvReflectCreateShaderModule(binary.size(), binary.data(), &_reflect) != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not preform reflection on a shader module!");
    }

    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = (uint32_t)binary.size();
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(binary.data());

    VK_CHECK(vkCreateShaderModule(_device->Get(), &moduleCreateInfo, nullptr, &_module))
}

Shader::~Shader() 
{ 
    vkDestroyShaderModule(_device->Get(), _module, nullptr); 
    spvReflectDestroyShaderModule(&_reflect);
}

VkShaderStageFlagBits Shader::GetStage() const 
{
    return _stage; 
}

const SpvReflectShaderModule& Shader::GetReflection() const
{
    return _reflect;
}

VkShaderModule Shader::Get() const 
{ 
    return _module;
}

} // namespace bl
