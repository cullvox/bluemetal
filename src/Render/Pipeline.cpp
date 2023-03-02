#include "Render/Pipeline.hpp"

#include <spirv_reflect.h>

namespace bl
{

Pipeline::Pipeline()
{

}

Pipeline::Pipeline(RenderDevice& renderDevice, const std::vector<uint32_t>& spirvCode, PipelineData& data)
    : m_pRenderDevice(&renderDevice)
{
    createLayout(spirvCode);
    createPipeline();
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(m_pRenderDevice->getDevice(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_pRenderDevice->getDevice(), m_pipelineLayout, nullptr);
}

void Pipeline::createLayout(const std::vector<uint32_t>& spirvCode)
{
    // Reflect the spirv provided by the constructor.
    SpvReflectShaderModule module{};
    spvReflectCreateShaderModule(spirvCode.size(), spirvCode.data(), &module);

    spvReflectEnumerateDescriptorSets(&module, );

    const VkPipelineLayoutCreateInfo layoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 
    };
}

} // namespace bl