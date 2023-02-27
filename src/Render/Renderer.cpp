#include "Render/Renderer.hpp"

#include <spdlog/spdlog.h>

namespace bl {

Renderer::Renderer(RenderDevice& renderDevice, Swapchain& swapchain)
    : m_renderDevice(renderDevice), m_swapchain(swapchain) 
{
    spdlog::info("Creating vulkan renderer.");
}

Renderer::~Renderer()
{
    spdlog::info("Destroying vulkan renderer.");
}

} /* namespace bl */