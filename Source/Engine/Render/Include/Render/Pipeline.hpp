#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Shader.hpp"
#include "Render/Precompiled.hpp"

class BLOODLUST_RENDER_API blPipeline
{

public:
    blPipeline(const std::shared_ptr<blRenderDevice> renderDevice,
        const std::vector<std::shared_ptr<blShader>>& shaders, 
        bool destroyShaders = false);
    ~blPipeline() noexcept;

private:
    void createLayout();
    void createPipeline();

    const std::shared_ptr<blRenderDevice> _renderDevice;

    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;
};