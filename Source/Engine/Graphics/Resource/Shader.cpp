#include "Shader.h"
#include "Conversions.h"
#include "Core/Print.h"

namespace bl {

GfxShader::GfxShader(
    std::shared_ptr<GfxDevice>      device,
    VkShaderStageFlagBits           stage,
    const std::vector<char>&    binary)
    : _device(device)
    , _stage(stage)
    , _module(VK_NULL_HANDLE)
{
    createModule(binary);
}

GfxShader::~GfxShader() 
{ 
    vkDestroyShaderModule(_device->get(), _module, nullptr); 
}

VkShaderModule GfxShader::get() const { return _module; }
VkShaderStageFlagBits GfxShader::getStage() const { return _stage; }

void GfxShader::createModule(const std::vector<char>& binary)
{
    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = (uint32_t)binary.size();
    moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(binary.data());

    VK_CHECK(vkCreateShaderModule(_device->get(), &moduleCreateInfo, nullptr, &_module))
}

} // namespace bl
