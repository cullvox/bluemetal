#pragma once

#include "RenderDevice.hpp"

namespace bl {

class Renderer
{
public:
    Renderer(RenderDevice& renderDevice);
    ~Renderer();

    void renderFrame();
    void presentFrame();    
private:
    RenderDevice& m_renderDevice;
    VkRenderPass m_pass;
};

} /* namespace bl */