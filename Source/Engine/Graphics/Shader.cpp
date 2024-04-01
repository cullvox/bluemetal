#include "Shader.h"
#include "Conversions.h"
#include "Core/Print.h"

namespace bl {

GfxShader::GfxShader(const CreateInfo& createInfo)
    : _device(createInfo.device)
    , _stage(createInfo.stage)
    , _module(VK_NULL_HANDLE)
{
    createModule(createInfo.binary);
}

GfxShader::~GfxShader() 
{ 
    vkDestroyShaderModule(_device->get(), _module, nullptr); 
}

VkShaderModule GfxShader::get() const { return _module; }
VkShaderStageFlagBits GfxShader::getStage() const { return _stage; }

void GfxShader::createModule(const std::vector<uint32_t>& binary)
{
    VkShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = (uint32_t)binary.size();
    moduleCreateInfo.pCode = binary.data();

    if (vkCreateShaderModule(_device->get(), &moduleCreateInfo, nullptr, &_module) != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Vulkan shader module!");
    }
}

} // namespace bl
