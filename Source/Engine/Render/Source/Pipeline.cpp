#include "Render/Pipeline.hpp"

#include <spirv_reflect.h>

blPipeline::blPipeline(RenderDevice& renderDevice, 
    const std::vector<blShader*>& shaders, bool deleteShaders) noexcept
{

    const VkPipelineLayoutCreateInfo layoutCreateInfo{
        
    }

}

Pipeline::~Pipeline() noexcept
{
    vkDestroyPipeline(m_pRenderDevice->getDevice(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_pRenderDevice->getDevice(), m_pipelineLayout, nullptr);
}

void Pipeline::createLayout()
{
    
}

void Pipeline::createPipeline()
{


}