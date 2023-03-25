#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Shader.hpp"

#include <glm/mat4x4.hpp>

#include <vector>
#include <filesystem>

namespace bl 
{

struct PipelineInfo
{
    const std::vector<Shader&>& stages;
};

struct FrameDescriptor
{
    float time; // in seconds
    Vector2i extent; // in pixels
};

struct ObjectDescriptor
{
    glm::mat4 cameraView;
    glm::mat4 cameraProjection;
    glm::mat4 modelTransform;
};

class BLOODLUST_API Pipeline 
{

public:
    Pipeline() noexcept;
    Pipeline(RenderDevice& renderDevice, std::vector<std::vector<uint32_t>> shaders) noexcept;
    ~Pipeline() noexcept;

    Pipeline& operator=(Pipeline&& rhs) noexcept;

    bool good() const noexcept;

private:
    void createLayout();
    void createPipeline();

    RenderDevice* m_pRenderDevice;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;
};

}; // namespace bl