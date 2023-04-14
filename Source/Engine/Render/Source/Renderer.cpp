#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Renderer.hpp"

blRenderer::blRenderer(const std::shared_ptr<blRenderDevice> renderDevice, 
        const std::shared_ptr<blSwapchain> swapchain,
        const std::vector<std::shared_ptr<blRenderPass>>& passes)
    : _renderDevice(renderDevice)
    , _swapchain(swapchain)
    , _passes(passes)
{

    buildSyncObjects();

}

void blRenderer::buildSyncObjects()
{

    const VkCommandBufferAllocateInfo allocationInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    };

    vkAllocateCommandBuffers(
        _renderDevice->getDevice(), 
        &allocationInfo, 
        _swapCommandBuffers.data())

}