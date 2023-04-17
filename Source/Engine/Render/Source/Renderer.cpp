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

void blRenderer::buildSyncObjects()
{

    const VkCommandBufferAllocateInfo allocationInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    };

    if (vkAllocateCommandBuffers(
            _renderDevice->getDevice(), 
            &allocationInfo, 
            _swapCommandBuffers.data())
        != VK_SUCCESS)
    {
        throw std::runtime_error("Could not allocate renderer command buffers!");
    }

}