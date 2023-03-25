#include "Render/Pipeline.hpp"

#include <spirv_reflect.h>

namespace bl
{

Pipeline::Pipeline() noexcept
{

}

Pipeline::Pipeline(RenderDevice& renderDevice, std::vector<std::vector<uint32_t>> shaders) noexcept
{

}

Pipeline::~Pipeline() noexcept
{
    vkDestroyPipeline(m_pRenderDevice->getDevice(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_pRenderDevice->getDevice(), m_pipelineLayout, nullptr);
}

void Pipeline::createLayout()
{
    //
}

void Pipeline::createPipeline()
{


}

} // namespace bl