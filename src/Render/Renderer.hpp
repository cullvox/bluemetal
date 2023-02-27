#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Swapchain.hpp"

namespace bl {

class Renderer
{
public:
    Renderer(RenderDevice& renderDevice, Swapchain& swapchain);
    ~Renderer();

    void renderFrame();
    void presentFrame();    
private:
    void createRenderPass();

    RenderDevice& m_renderDevice;
    Swapchain& m_swapchain;
    VkRenderPass m_pass;
};

} /* namespace bl */