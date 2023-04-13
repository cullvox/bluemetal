#include "Render/Pipeline.hpp"

#include <spirv_reflect.h>

blPipeline::blPipeline(std::shared_ptr<blRenderDevice> renderDevice, 
    const std::vector<std::shared_ptr<blShader>>& shaders, bool deleteShaders)
    : _renderDevice(renderDevice)
{
}

blPipeline::~blPipeline() noexcept
{
    vkDestroyPipeline(_renderDevice->getDevice(), _pipeline, nullptr);
    vkDestroyPipelineLayout(_renderDevice->getDevice(), _pipelineLayout, nullptr);
}

void blPipeline::createLayout()
{
}

void blPipeline::createPipeline()
{
}