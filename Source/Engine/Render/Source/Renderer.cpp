#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Renderer.hpp"

blRenderer::blRenderer(std::shared_ptr<const blRenderDevice> renderDevice, 
        std::shared_ptr<const blSwapchain> swapchain,
        const std::vector<std::shared_ptr<blRenderPass>>& passes)
    : _renderDevice(renderDevice)
    , _swapchain(swapchain)
    , _passes(passes)
{

    buildSyncObjects();

}

blRenderer::~blRenderer()
{

}

void blRenderer::buildSyncObjects()
{

    const vk::CommandBufferAllocateInfo allocationInfo
    {
        _renderDevice->getCommandPool(),    // commandPool 
        vk::CommandBufferLevel::ePrimary,   // level
        _swapchain->getImageCount()         // commandBufferCount
    };

    _swapCommandBuffers = _renderDevice->getDevice()
        .allocateCommandBuffers(allocationInfo);

}

void blRenderer::render()
{
    for (auto pass : _passes)
    {
        pass->record();
    }
}