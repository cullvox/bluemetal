#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Renderer.hpp"
#include <vulkan/vulkan_structs.hpp>

blRenderer::blRenderer(std::shared_ptr<blRenderDevice> renderDevice, std::shared_ptr<blSwapchain> swapchain, std::vector<std::shared_ptr<blRenderPass>>& passes)
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

    // Allocate the per frame - swapping command buffers. 
    const vk::CommandBufferAllocateInfo allocationInfo
    {
        _renderDevice->getCommandPool(),    // commandPool 
        vk::CommandBufferLevel::ePrimary,   // level
        MAX_FRAMES_IN_FLIGHT                // commandBufferCount
    };

    _swapCommandBuffers = _renderDevice->getDevice().allocateCommandBuffers(allocationInfo);


    // Create the semaphores and fences
    _imageAvailableSemaphores.resize(_swapchain->getImageCount());
    _renderFinishedSemaphores.resize(_swapchain->getImageCount());
    _inFlightFences.resize(_swapchain->getImageCount());

    const vk::SemaphoreCreateInfo semaphoreInfo{};
    const vk::FenceCreateInfo fenceInfo
    {
        vk::FenceCreateFlagBits::eSignaled
    };

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        _imageAvailableSemaphores[i] = _renderDevice->getDevice().createSemaphore(semaphoreInfo);
        _renderFinishedSemaphores[i] = _renderDevice->getDevice().createSemaphore(semaphoreInfo);
        _inFlightFences[i] = _renderDevice->getDevice().createFence(fenceInfo);
    }
}

void blRenderer::destroySyncObjects() noexcept
{
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        _renderDevice->getDevice().destroySemaphore(_imageAvailableSemaphores[i]);
        _renderDevice->getDevice().destroySemaphore(_renderFinishedSemaphores[i]);
        _renderDevice->getDevice().destroyFence(_inFlightFences[i]);
    }

    _renderDevice->getDevice().freeCommandBuffers(_renderDevice->getCommandPool(), _swapCommandBuffers);
}

void blRenderer::resize(blExtent2D extent)
{
    for (auto pass : _passes)
    {
        pass->resize((vk::Extent2D)extent);
    }
}

void blRenderer::render()
{

    // If the swapchain is invalid we cannot render anything
    if (!_swapchain->getSwapchain())
    {
        return;
    }

    std::ignore = _renderDevice->getDevice().waitForFences( {_inFlightFences[_currentFrame]}, VK_TRUE, UINT64_MAX);

    // Get the specific command buffer and image frame to use.
    uint32_t imageIndex = 0;
    bool recreated = false;
    
    _swapchain->acquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, imageIndex, recreated);

    if (recreated)
    {
        resize(_swapchain->getExtent());
    }

    _renderDevice->getDevice()
        .resetFences({_inFlightFences[_currentFrame]});
    
    // Record each render pass
    vk::CommandBufferBeginInfo beginInfo{};
    _swapCommandBuffers[_currentFrame].reset();
    _swapCommandBuffers[_currentFrame].begin(beginInfo);

    const vk::Rect2D renderArea
    {
        {0, 0},
        _swapchain->getVulkanExtent()
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
    const std::array commandBuffers = { _swapCommandBuffers[_currentFrame] };

    const vk::SubmitInfo submitInfo
    {
        waitSemaphores,     // waitSemaphores
        waitStages,         // waitDstStageMask
        commandBuffers,     // commandBuffers
        signalSemaphores    // signalSemaphores
    };

    _renderDevice->getGraphicsQueue()
        .submit({submitInfo}, _inFlightFences[_currentFrame]);

    // Present the rendered frame
    const std::array swapchains = { _swapchain->getSwapchain() };
    const std::array imageIndices = { imageIndex };
    const vk::PresentInfoKHR presentInfo
    {
        signalSemaphores,
        swapchains,
        imageIndices,
        {}
    };

    try
    {
        std::ignore = _renderDevice->getPresentQueue().presentKHR(presentInfo);
    } 
    catch (const vk::OutOfDateKHRError&)
    {
        // Gotta recreate the swapchain
        _swapchain->recreate();
        resize(_swapchain->getExtent());
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}