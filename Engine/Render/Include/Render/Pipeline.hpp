#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Shader.hpp"

#include <vector>

namespace bl {

struct PipelineInfo
{
    const std::vector<Shader&>& stages;
};

class Pipeline {
public:
    Pipeline();
    Pipeline(RenderDevice& renderDevice, PipelineInfo& info);
    ~Pipeline();
private:
    void createLayout();
    void createPipeline();

    RenderDevice* m_pRenderDevice;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;
};

}; // namespace bl