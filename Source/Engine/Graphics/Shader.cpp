#include "Shader.h"
#include "Conversions.h"
#include "Core/Print.h"

namespace bl {

Shader::Shader(const nlohmann::json& json, Device* device)
    : Resource(json)
    , _device(device)
    , _reflect()
    , _module(VK_NULL_HANDLE)
{
    // Determine the shaders stage from the json
    auto stage = json["Stage"].get<std::string>();
    if (stage == "Vertex")
    {
        _stage = VK_SHADER_STAGE_VERTEX_BIT;
    }
    else if (stage == "Fragment")
    {
        _stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    else
    {
        throw std::runtime_error("Invalid shader stage!");
    }
}

Shader::~Shader() 
{ 
    Unload();
}

void Shader::Load()
{
    // Load the shader binary into memory.
    std::ifstream file(GetPath(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) 
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    // Create the reflection module for pipeline usage.
    if (spvReflectCreateShaderModule(buffer.size(), buffer.data(), &_reflect) != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not preform reflection on a shader module!");
    }

    // Create the shader module.
    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = (uint32_t)buffer.size();
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    VK_CHECK(vkCreateShaderModule(_device->Get(), &moduleCreateInfo, nullptr, &_module))
}

void Shader::Unload()
{
    if (_module == VK_NULL_HANDLE) return;
     
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
