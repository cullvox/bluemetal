#include "Renderer.h"
#include "Core/Print.h"

namespace bl {


Renderer::Renderer(
    std::shared_ptr<GfxDevice>      device,
    std::shared_ptr<Swapchain>   swapchain)
    : _device(device)
    , _swapchain(swapchain)
    , _currentFrame(0)
{
    _swapCommandBuffers.resize(GfxLimits::maxFramesInFlight);
    _imageAvailableSemaphores.resize(_swapchain->getImageCount());
    _renderFinishedSemaphores.resize(_swapchain->getImageCount());
    _inFlightFences.resize(_swapchain->getImageCount());

    _presentPass = std::make_shared<PresentPass>(_device, _swapchain);

    createSyncObjects();
}

Renderer::~Renderer() 
{
    _device->waitForDevice();

    destroySyncObjects();
}

std::shared_ptr<RenderPass> Renderer::getUserInterfacePass() { return _presentPass; }

void Renderer::createSyncObjects()
{
    // Allocate the per frame swapping command buffers.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->getCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = GfxLimits::maxFramesInFlight;

    VK_CHECK(vkAllocateCommandBuffers(_device->get(), &allocateInfo, _swapCommandBuffers.data()))

    // Create the semaphores and fences
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < GfxLimits::maxFramesInFlight; i++) {
        VK_CHECK(vkCreateSemaphore(_device->get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]))
        VK_CHECK(vkCreateSemaphore(_device->get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
        VK_CHECK(vkCreateFence(_device->get(), &fenceInfo, nullptr, &_inFlightFences[i]))
    }
}

void Renderer::destroySyncObjects()
{
    for (uint32_t i = 0; i < GfxLimits::maxFramesInFlight; i++) {
        vkDestroySemaphore(_device->get(), _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_device->get(), _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(_device->get(), _inFlightFences[i], nullptr);
    }

    vkFreeCommandBuffers(_device->get(), _device->getCommandPool(), (uint32_t)_swapCommandBuffers.size(), _swapCommandBuffers.data());
}

void Renderer::resize(VkExtent2D extent)
{
    // _geometryPass->recreate(extent);
    _presentPass->recreate(extent);
}

void Renderer::render(std::function<void(VkCommandBuffer)> func)
{
    // If the swapchain is invalid we cannot render anything.
    if (!_swapchain->get())
        return;

    // Wait for the current image up coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX))

    // Acquire the next image in the swapchain.
    uint32_t imageIndex = 0;
    bool recreated = false;
    _swapchain->acquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, imageIndex, recreated);

    // Resize passes images if the swapchain resized.
    if (recreated) {
        resize(_swapchain->getExtent());
        return; // skip this frame!
    }

    // Reset the fence for this image so it can signal when it's done.
    VK_CHECK(vkResetFences(_device->get(), 1, &_inFlightFences[_currentFrame]))

    // Record each render pass
    auto cmd = _swapCommandBuffers[_currentFrame];
    VK_CHECK(vkResetCommandBuffer(cmd, 0))

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    // Record the render passes.
    VkRect2D renderArea { { 0, 0 }, _swapchain->getExtent() };
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo))
    _presentPass->begin(cmd, renderArea, imageIndex);
    func(cmd);
    _presentPass->end(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd))

    // Submit the command buffer to the graphics queue.
    std::array waitSemaphores = { _imageAvailableSemaphores[_currentFrame] };
    std::array signalSemaphores = { _renderFinishedSemaphores[_currentFrame] };
    std::array commandBuffers = { _swapCommandBuffers[_currentFrame] };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = (uint32_t)commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    VK_CHECK(vkQueueSubmit(_device->getGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]))



    // Set the next frames index.
    _currentFrame = (_currentFrame + 1) % GfxLimits::maxFramesInFlight;
}

} // namespace bl
