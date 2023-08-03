#include "Renderer.h"
#include "Core/Log.h"

namespace bl
{

Renderer::Renderer(std::shared_ptr<Device> device, std::shared_ptr<Swapchain> swapchain)
    : m_device(device)
    , m_swapchain(swapchain)
    , m_currentFrame(0)
    , m_presentPass(std::make_shared<PresentRenderPass>(device, swapchain))
{
    createSyncObjects();
}

Renderer::~Renderer()
{
    destroySyncObjects();
}

std::shared_ptr<RenderPass> Renderer::getImGuiPass()
{
    return m_presentPass;
}

void Renderer::createSyncObjects()
{

    // allocate the per frame swapping command buffers. 
    m_swapCommandBuffers.resize(maxFramesInFlight);

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = m_device->getCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = maxFramesInFlight;

    if (vkAllocateCommandBuffers(m_device->getHandle(), &allocateInfo, m_swapCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not allocate Vulkan command buffers for the renderer!");
    }


    // Create the semaphores and fences
    m_imageAvailableSemaphores.resize(m_swapchain->getImageCount());
    m_renderFinishedSemaphores.resize(m_swapchain->getImageCount());
    m_inFlightFences.resize(m_swapchain->getImageCount());

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < maxFramesInFlight; i++)
    {
        if (vkCreateSemaphore(m_device->getHandle(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device->getHandle(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device->getHandle(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a Vulkan sync object!");
        }
    }
}

void Renderer::destroySyncObjects() noexcept
{
    for (uint32_t i = 0; i < maxFramesInFlight; i++)
    {
        vkDestroySemaphore(m_device->getHandle(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_device->getHandle(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_device->getHandle(), m_inFlightFences[i], nullptr);
    }

    vkFreeCommandBuffers(m_device->getHandle(), m_device->getCommandPool(), (uint32_t)m_swapCommandBuffers.size(), m_swapCommandBuffers.data());
}

void Renderer::resize(VkExtent2D extent)
{
    m_presentPass->resize(extent);
}

void Renderer::render()
{
    // If the swapchain is invalid we cannot render anything.
    if (!m_swapchain->getHandle()) return;

    // Wait for the current image up coming in the chain to finish.
    vkWaitForFences(m_device->getHandle(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire the next image in the swapchain.
    uint32_t imageIndex = 0;
    bool recreated = false;

    m_swapchain->acquireNext(m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex, &recreated);

    // Resize passes images if the swapchain resized. 
    if (recreated)
    {
        resize(m_swapchain->getExtent());
    }

    // Reset the fence for this image so it can signal when it's done.
    vkResetFences(m_device->getHandle(), 1, &m_inFlightFences[m_currentFrame]);
    
    // Record each render pass
    vkResetCommandBuffer(m_swapCommandBuffers[m_currentFrame], 0);
    
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(m_swapCommandBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not begin a Vulkan command buffer!");
    }

    VkRect2D renderArea
    {
        {0, 0},
        m_swapchain->getExtent()
    };

    // Record the render passes.
    m_presentPass->record(m_swapCommandBuffers[m_currentFrame], renderArea, imageIndex);

    if (vkEndCommandBuffer(m_swapCommandBuffers[m_currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not end a Vulkan command buffer!");
    }

    // Submit the command buffer to the graphics queue.
    std::array waitSemaphores = { m_imageAvailableSemaphores[m_currentFrame] };
    std::array signalSemaphores = { m_renderFinishedSemaphores[m_currentFrame] };
    std::array commandBuffers = { m_swapCommandBuffers[m_currentFrame] };

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

    vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]);

    // Queue the frame to be presented.
    std::array swapchains = { m_swapchain->getHandle() };
    std::array imageIndices = { imageIndex };

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = (uint32_t)signalSemaphores.size();
    presentInfo.pWaitSemaphores = signalSemaphores.data();
    presentInfo.swapchainCount = (uint32_t)swapchains.size();
    presentInfo.pSwapchains = swapchains.data();
    presentInfo.pImageIndices = imageIndices.data();
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // Tell the swapchain that the images need to be recreated.
        m_swapchain->recreate(m_swapchain->getPresentMode());
        resize(m_swapchain->getExtent());
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Could not queue Vulkan present!");
    }

    // Set the next frames index.
    m_currentFrame = (m_currentFrame + 1) % maxFramesInFlight;
}

} // namespace bl