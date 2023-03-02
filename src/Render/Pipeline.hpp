#pragma once

#include "Render/RenderDevice.hpp"

namespace bl {

struct PipelineData
{
};

class Pipeline{
public:
    Pipeline();
    Pipeline(RenderDevice& renderDevice, const std::vector<uint32_t>& code, PipelineData& data);
    ~Pipeline();
private:
    void createLayout(const std::vector<uint32_t>& spirvCode);
    void createPipeline();

    RenderDevice* m_pRenderDevice;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;
};

}; // namespace bl