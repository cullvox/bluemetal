#include "Render/Renderer.hpp"

namespace bl {

Renderer::Renderer(RenderDevice& renderDevice, Swapchain& swapchain)
    : m_renderDevice(renderDevice), m_swapchain(swapchain) 
{
}

Renderer::~Renderer()
{
}

} /* namespace bl */