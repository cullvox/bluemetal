#include "Shader.h"
#include "Conversions.h"
#include "Core/Print.h"

namespace bl {

Shader::Shader()
{
}

Shader::Shader(Shader&& rhs)
{
    _pDevice = std::move(rhs._pDevice);
    _stage = std::move(rhs._stage);
    _binary = std::move(rhs._binary);
    _module = std::move(rhs._module);

    rhs._pDevice = nullptr;
    rhs._stage = 0;
    rhs._binary = {};
    rhs._module = VK_NULL_HANDLE;
}

Shader::Shader(const ShaderCreateInfo& info)
{
    if (!create(info)) throw std::runtime_error("Could not create a shader, check logs.");
}

Shader::~Shader() 
{ 
    destroy();
}

bool Shader::create(const ShaderCreateInfo& info)
{
    _pDevice = info.pDevice;
    _stage = info.stage;
    _binary = info.binary;
    
    return createModule();
}

void Shader::destroy() const noexcept
{
    if (!isCreated()) return;
    
    vkDestroyShaderModule(_pDevice->getHandle(), _module, nullptr); 
}

bool Shader::isCreated() const noexcept
{
    return _module == VK_NULL_HANDLE;
}

VkShaderStageFlagBits Shader::getStage() { return _stage; }
VkShaderModule Shader::getHandle() { return _module; }
std::vector<uint32_t> Shader::getBinary() { return _binary; }

bool Shader::createModule() noexcept
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = (uint32_t)_binary.size();
    createInfo.pCode = _binary.data();

    if (vkCreateShaderModule(_pDevice->getHandle(), &createInfo, nullptr, &_module) != VK_SUCCESS) {
        blError("Could not create Vulkan shader module!");
        return false;
    }

    return true;
}

} // namespace bl
