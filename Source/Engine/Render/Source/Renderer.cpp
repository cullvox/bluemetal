#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Renderer.hpp"

blRenderer::blRenderer(std::shared_ptr<const blRenderDevice> renderDevice, 
        std::shared_ptr<blSwapchain> swapchain,
        const std::vector<std::shared_ptr<blRenderPass>>& passes)
    : _renderDevice(renderDevice)
    , _swapchain(swapchain)
    , _passes(passes)
    , _currentFrame(0)
{

    createSyncObjects();

}

blRenderer::~blRenderer()
{
    destroySyncObjects();
}

void blRenderer::createSyncObjects()
{

    const vk::CommandBufferAllocateInfo allocationInfo
    {
        _renderDevice->getCommandPool(),    // commandPool 
        vk::CommandBufferLevel::ePrimary,   // level
        MAX_FRAMES_IN_FLIGHT                // commandBufferCount
    };

    _swapCommandBuffers = _renderDevice->getDevice()
        .allocateCommandBuffers(allocationInfo);


    _imageAvailableSemaphores.resize(_swapchain->getImageCount());
    _renderFinishedSemaphores.resize(_swapchain->getImageCount());
    _inFlightFences.resize(_swapchain->getImageCount());

    const vk::SemaphoreCreateInfo semaphoreInfo
    {
    };

    const vk::FenceCreateInfo fenceInfo
    {
        vk::FenceCreateFlagBits::eSignaled
    };

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        std::ignore = _renderDevice->getDevice()
            .createSemaphore(&semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]);
        std::ignore = _renderDevice->getDevice()
            .createSemaphore(&semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]);
        std::ignore = _renderDevice->getDevice()
            .createFence(&fenceInfo, nullptr, &_inFlightFences[i]);
    }
}

void blRenderer::destroySyncObjects() noexcept
{
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        _renderDevice->getDevice()
            .destroySemaphore(_imageAvailableSemaphores[i]);
        _renderDevice->getDevice()
            .destroySemaphore(_renderFinishedSemaphores[i]);
        _renderDevice->getDevice()
            .destroyFence(_inFlightFences[i]);
    }

    _renderDevice->getDevice()
        .freeCommandBuffers(
            _renderDevice->getCommandPool(),
            _swapCommandBuffers);
}

void blRenderer::resize(vk::Extent2D extent)
{
    for (auto pass : _passes)
    {
        pass->resize(extent);
    }
}

void blRenderer::render()
{
    auto fenceResult = _renderDevice->getDevice()
        .waitForFences( {_inFlightFences[_currentFrame]}, VK_TRUE, UINT64_MAX);

    // Get the specific command buffer and image frame to use.
    uint32_t imageIndex = 0;
    bool recreated = false;
    
    _swapchain->acquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, imageIndex, recreated);

    if (recreated)
    {
        resize(_swapchain->getExtentVk());
    }

    _renderDevice->getDevice()
        .resetFences({_inFlightFences[_currentFrame]});
    
    // Record each render pass
    _swapCommandBuffers[_currentFrame].reset();
    const vk::CommandBufferBeginInfo beginInfo
    {
    };
    
    _swapCommandBuffers[_currentFrame].begin(beginInfo);

    const vk::Rect2D renderArea
    {
        {0, 0},
        _swapchain->getExtentVk()
    };

    for (auto pass : _passes)
    {
        pass->record(_swapCommandBuffers[_currentFrame], renderArea, imageIndex);
    }

    _swapCommandBuffers[_currentFrame].end();


    // Submit the command buffer to the graphics queue
    const std::array waitSemaphores = { _imageAvailableSemaphores[_currentFrame] };
    const std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    const std::array signalSemaphores = { _renderFinishedSemaphores[_currentFrame] };

    const vk::SubmitInfo submitInfo
    {
        waitSemaphores,                          // waitSemaphores
        waitStages,                             // waitDstStageMask
        { _swapCommandBuffers[_currentFrame] }, // commandBuffers
        signalSemaphores                        // signalSemaphores
    };

    _renderDevice->getGraphicsQueue()
        .submit({submitInfo}, _inFlightFences[_currentFrame]);

    // Present the rendered frame
    const std::array swapchains = { _swapchain->getSwapchain() };
    const vk::PresentInfoKHR presentInfo
    {
        signalSemaphores,
        swapchains,
        { imageIndex },
        {}
    };

    try
    {
        std::ignore = _renderDevice->getPresentQueue()
            .presentKHR(presentInfo);
    } 
    catch (const vk::OutOfDateKHRError& e)
    {
        // Gotta recreate the swapchain
        _swapchain->recreate();
        resize(_swapchain->getExtentVk());
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}