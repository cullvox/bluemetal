#include "Renderer.h"
#include "Core/Print.h"

namespace bl {


Renderer::Renderer(Device* device, Window* window)
    : _device(device)
    , _window(window)
    , _swapchain(window->GetSwapchain())
    , _currentFrame(0)
{
    _presentPass = std::make_unique<PresentPass>(_device, _swapchain);
    _commandBuffers.resize(GraphicsConfig::numFramesInFlight);
    _imageAvailableSemaphores.resize(_swapchain->GetImageCount());
    _renderFinishedSemaphores.resize(_swapchain->GetImageCount());
    _inFlightFences.resize(_swapchain->GetImageCount());

    CreateSyncObjects();
}

Renderer::~Renderer() 
{
    _device->WaitForDevice();
    DestroySyncObjects();
}

RenderPass* Renderer::GetUIPass() 
{ 
    return _presentPass.get(); 
}

void Renderer::CreateSyncObjects()
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = GraphicsConfig::numFramesInFlight;

    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, _commandBuffers.data()))

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < GraphicsConfig::numFramesInFlight; i++) {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]))
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &_inFlightFences[i]))
    }
}

void Renderer::DestroySyncObjects()
{
    for (uint32_t i = 0; i < GraphicsConfig::numFramesInFlight; i++) {
        vkDestroySemaphore(_device->Get(), _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_device->Get(), _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(_device->Get(), _inFlightFences[i], nullptr);
    }

    vkFreeCommandBuffers(_device->Get(), _device->GetCommandPool(), (uint32_t)_commandBuffers.size(), _commandBuffers.data());
}

Window* Renderer::GetWindow()
{
    return _window;
}

void Renderer::Recreate()
{

    VkExtent2D extent = _swapchain->GetExtent();
    // _geometryPass->recreate(extent);
    _presentPass->Recreate(extent);
}

void Renderer::Render(std::function<void(VkCommandBuffer, uint32_t)> func)
{
    if (!_swapchain->Get()) // Swapchain must be valid.
        return;

    // Wait for the current image up coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->Get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX))

    
    // Acquire the next image in the swapchain and Resize passes images if the swapchain resized.
    if (_swapchain->AcquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE)) 
    {
        Recreate();
        return; // skip this frame!
    }

    auto imageIndex = _swapchain->GetImageIndex();

    // Reset the fence for this image so it can signal when it's done.
    VK_CHECK(vkResetFences(_device->Get(), 1, &_inFlightFences[_currentFrame]))

    auto cmd = _commandBuffers[_currentFrame];
    VK_CHECK(vkResetCommandBuffer(cmd, 0))

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VkRect2D renderArea{ { 0, 0 }, _swapchain->GetExtent() };

    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo))
    _presentPass->Begin(cmd, renderArea, imageIndex);
    func(cmd, _currentFrame);
    _presentPass->End(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd))

    // Submit the command buffer to the graphics queue.
    std::array waitSemaphores = { _imageAvailableSemaphores[_currentFrame] };
    std::array signalSemaphores = { _renderFinishedSemaphores[_currentFrame] };
    std::array commandBuffers = { _commandBuffers[_currentFrame] };
    std::array waitStages = { (VkPipelineStageFlags)VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = (uint32_t)commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    VK_CHECK(vkQueueSubmit(_device->GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]))

    if (_swapchain->QueuePresent(_renderFinishedSemaphores[_currentFrame]))
    {
        Recreate();
    }

    _currentFrame = (_currentFrame + 1) % GraphicsConfig::numFramesInFlight;
}

} // namespace bl