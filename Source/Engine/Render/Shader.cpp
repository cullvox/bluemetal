#include "Shader.h"
#include "FormatConversions.h"
#include "Core/Log.h"

namespace bl
{

Shader::Shader(std::shared_ptr<Device> device, VkShaderStageFlagBits stage, const std::vector<uint32_t>& binary)
    : m_device(device)
    , m_stage(stage)
    , m_binary(binary)
{
    createModule();
}

Shader::~Shader()
{
    vkDestroyShaderModule(m_device->getHandle(), m_module, nullptr);
} 

VkShaderStageFlagBits Shader::getStage()
{
    return m_stage;
}

VkShaderModule Shader::getHandle()
{
    return m_module;
}

std::vector<uint32_t> Shader::getBinary()
{
    return m_binary;
}

void Shader::createModule()
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = (uint32_t)m_binary.size();
    createInfo.pCode = m_binary.data();

    if (vkCreateShaderModule(m_device->getHandle(), &createInfo, nullptr, &m_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create Vulkan shader module!");
    }
}

} // namespace bl