#include "Render/Pipeline.hpp"

#include <spirv_reflect.h>

namespace bl
{

Pipeline::Pipeline()
{

}

Pipeline::Pipeline(RenderDevice& renderDevice, PipelineInfo& info)
    : m_pRenderDevice(&renderDevice)
{
    createLayout();
    createPipeline();
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(m_pRenderDevice->getDevice(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_pRenderDevice->getDevice(), m_pipelineLayout, nullptr);
}

void Pipeline::createLayout()
{
    // Reflect the spirv provided by the constructor.
    /*
    SpvReflectShaderModule module{};
    spvReflectCreateShaderModule(spirvCode.size(), spirvCode.data(), &module);

    spvReflectEnumerateDescriptorSets(&module, );

    const VkPipelineLayoutCreateInfo layoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 
    };*/
}

void Pipeline::createPipeline()
{
}

} // namespace bl