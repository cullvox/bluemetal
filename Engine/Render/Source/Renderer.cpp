#include "Core/Log.hpp"
#include "Render/Renderer.hpp"

#include <array>

namespace bl 
{

Renderer::Renderer() noexcept
    : m_pRenderDevice(nullptr)
    , m_pSwapchain(nullptr)
    , m_pass(VK_NULL_HANDLE)
    , m_currentFrame(0)
    , m_previousFrame(0)
    , m_imageIndex(0)
    , m_framebufferResized(false)
    , m_deadFrame(false)
    , m_swapCommandBuffers({})
    , m_swapImageViews({})
    , m_swapFramebuffers({})
    , m_imageAvailableSemaphores({})
    , m_renderFinishedSemaphores({})
    , m_inFlightFences({})
{
}

Renderer::Renderer(RenderDevice& renderDevice, Swapchain& swapchain) noexcept
    : Renderer()
{
    m_pRenderDevice = &renderDevice;
    m_pSwapchain = &swapchain;

    if (not m_pRenderDevice->good() ||
        not m_pSwapchain->good())
    {
        Logger::Error("Cannot construct the renderer parameter was invalid!");
        return;
    }

    /* Create the swapchain based objects and the render pass. */
    createRenderPass();
    recreateSwappable();
}

Renderer::~Renderer()
{
    collapse();
}

Renderer& Renderer::operator=(Renderer&& rhs) noexcept
{
    collapse();

    m_pRenderDevice = std::move(rhs.m_pRenderDevice);
    m_pSwapchain = std::move(rhs.m_pSwapchain);
    m_depthFormat = rhs.m_depthFormat;
    m_depthImage = std::move(rhs.m_depthImage);
    m_pass = rhs.m_pass;
    m_currentFrame = rhs.m_currentFrame;
    m_previousFrame = rhs.m_previousFrame;
    m_imageIndex = rhs.m_imageIndex;
    m_framebufferResized = rhs.m_framebufferResized;
    m_deadFrame = rhs.m_deadFrame;
    m_swapCommandBuffers = rhs.m_swapCommandBuffers;
    m_swapImageViews = rhs.m_swapImageViews;
    m_swapFramebuffers = rhs.m_swapFramebuffers;
    m_imageAvailableSemaphores = rhs.m_imageAvailableSemaphores;
    m_renderFinishedSemaphores = rhs.m_renderFinishedSemaphores;
    m_inFlightFences = rhs.m_inFlightFences;

    rhs.m_pRenderDevice = nullptr;
    rhs.m_pSwapchain = nullptr;
    rhs.m_depthFormat = VK_FORMAT_UNDEFINED;
    rhs.m_pass = VK_NULL_HANDLE;
    rhs.m_currentFrame = 0;
    rhs.m_previousFrame = 0;
    rhs.m_imageIndex = 0;
    rhs.m_framebufferResized = false;
    rhs.m_deadFrame = false;
    rhs.m_swapCommandBuffers.clear();
    rhs.m_swapImageViews.clear();
    rhs.m_swapFramebuffers.clear();
    rhs.m_imageAvailableSemaphores.clear();
    rhs.m_renderFinishedSemaphores.clear();
    rhs.m_inFlightFences.clear();

    return *this;
}

void Renderer::submit(const Submission& submission) noexcept
{
    //vkCmdBindPipeline(m_swapCommandBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, submission.pipeline);
}

bool Renderer::good() const noexcept
{
    return (m_pRenderDevice->good() &&
        m_depthImage.good() &&
        m_swapCommandBuffers.size() > 0 &&
        m_swapFramebuffers.size() > 0 &&
        m_swapImageViews.size() > 0 &&
        m_imageAvailableSemaphores.size() > 0 &&
        m_renderFinishedSemaphores.size() > 0 &&
        m_inFlightFences.size() > 0);
}

bool Renderer::beginFrame() noexcept
{

    // If the swapchain wasn't recreated for some reason skip the frame.
    if (!m_pSwapchain->good())
    {
        m_deadFrame = true;
        return true;
    }

    vkWaitForFences(m_pRenderDevice->getDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire the next image from the swapchain.
    bool requiresRecreation = false;
    if (not m_pSwapchain->acquireNext(m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, m_imageIndex, requiresRecreation))
    {
        Logger::Error("Could not acquire the swapchains next image!\n");
        return false;
    }

    // Attempt to recreate the swapchain and if it fails we need to 
    if (requiresRecreation)
    {
        if (not m_pSwapchain->recreate() ||
            not recreateSwappable())
        {
            Logger::Error("Could not recreate the swapchain and swappables!\n");
            return false;
        }
    }

    vkResetFences(m_pRenderDevice->getDevice(), 1, &m_inFlightFences[m_currentFrame]);

    // Reset the prevous frames command buffer.
    vkResetCommandBuffer(m_swapCommandBuffers[m_currentFrame], 0);

    // Begin the command buffer for this frame, submissions are open.
    const VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };

    if (vkBeginCommandBuffer(m_swapCommandBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS)
    {
        Logger::Error("Could not begin a command buffer after this frame!\n");
        return false;
    }

    // Setup the render pass
    const std::array<VkClearValue, 2> clearValues{ {
        {
            .color = {
                .float32 = { 0.596f, 0.757f, 0.851f, 1.0f }
            }
        },
        {
            .depthStencil = {
                .depth = 0.0f,
                .stencil = 0
            }
        }
    } };

    const Extent2D extent = m_pSwapchain->getExtent();
    const VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = m_pass,
        .framebuffer = m_swapFramebuffers[m_imageIndex],
        .renderArea = { 
            {0, 0}, // offset
            {(uint32_t)extent.width, (uint32_t)extent.height}, //extent
        },
        .clearValueCount = (uint32_t)clearValues.size(),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass(m_swapCommandBuffers[m_currentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    return true;
}

bool Renderer::endFrame() noexcept
{
    // If this frame is considered dead don't do anything and goto next frame.
    if (m_deadFrame)
    {
        m_deadFrame = false;
        return true;
    }

    // End the previous command buffer.
    vkCmdEndRenderPass(m_swapCommandBuffers[m_currentFrame]);
    vkEndCommandBuffer(m_swapCommandBuffers[m_currentFrame]);

    // Submit the command buffer for rendering.
    const VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    const VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &m_swapCommandBuffers[m_currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    if (vkQueueSubmit(m_pRenderDevice->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
    {
        Logger::Error("Could not submit a draw command buffer!\n");
        return false;
    }

    VkResult result = {};

    // Present the rendered image.
    const VkSwapchainKHR swapChains[] = {m_pSwapchain->getSwapchain()};
    const VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &m_imageIndex,
        .pResults = nullptr,
    };

    result = vkQueuePresentKHR(m_pRenderDevice->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        // Recreate the swapchain next frame.
    }
    else if (result != VK_SUCCESS)
    {
        Logger::Error("Could not queue the vulkan present!");
        return false;
    }

    // Move the frame number to the next in sequence.
    m_currentFrame = (m_currentFrame + 1) % DEFAULT_FRAMES_IN_FLIGHT;

    return true;
}

bool Renderer::createRenderPass() noexcept
{
    m_depthFormat = m_pRenderDevice->findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    const std::array<VkAttachmentDescription, 2> attachmentDescriptions = {{
        { // Color Attachment
            0, // flags
            m_pSwapchain->getFormat(), // format
            VK_SAMPLE_COUNT_1_BIT, // samples
            VK_ATTACHMENT_LOAD_OP_CLEAR, // loadOp
            VK_ATTACHMENT_STORE_OP_STORE, // storeOp
            VK_ATTACHMENT_LOAD_OP_DONT_CARE, // stencilLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE, // stencilStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED, // initialLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // finalLayout
        },
        { // Depth Attachment
            0, // flags
            m_depthFormat, // format
            VK_SAMPLE_COUNT_1_BIT, // samples
            VK_ATTACHMENT_LOAD_OP_CLEAR, // loadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE, // storeOp
            VK_ATTACHMENT_LOAD_OP_CLEAR, // stencilLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE, // stencilStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED, // initialLayout
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // finalLayout
        }
    }};

    const VkAttachmentReference colorAttachmentReference{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    
    const std::array<VkSubpassDescription, 1> subpassDescriptions{
        {
            0, // flags
            VK_PIPELINE_BIND_POINT_GRAPHICS, // pipelineBindPoint
            0, // inputAttachmentCount
            nullptr, // pInputAttachments
            1, // colorAttachmentCount
            &colorAttachmentReference, // pColorAttachments
            nullptr, // pResolveAttachments
            nullptr, // pDepthStencilAttachment
            0, // preserveAttachmentCount
            nullptr, // pPreserveAttachments
        }
    };

    const std::array<VkSubpassDependency, 0> subpassDependencies{};

    const VkRenderPassCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = (uint32_t)attachmentDescriptions.size(),
        .pAttachments = attachmentDescriptions.data(),
        .subpassCount = (uint32_t)subpassDescriptions.size(),
        .pSubpasses = subpassDescriptions.data(),
        .dependencyCount = (uint32_t)subpassDependencies.size(),
        .pDependencies = subpassDependencies.data()
    };

    if (vkCreateRenderPass(m_pRenderDevice->getDevice(), &createInfo, nullptr, &m_pass) != VK_SUCCESS)
    {
        Logger::Error("Could not create a vulkan render pass!");
        return false;
    }

    return true;
}

bool Renderer::createCommandBuffers() noexcept
{
    const VkCommandBufferAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_pRenderDevice->getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = m_pSwapchain->getImageCount(),
    };

    m_swapCommandBuffers.resize(m_pSwapchain->getImageCount());

    if (vkAllocateCommandBuffers(m_pRenderDevice->getDevice(), &allocateInfo, m_swapCommandBuffers.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not allocate vulkan swapchain command buffers!\n");
        return false;
    }

    return true;
}

bool Renderer::createFrameBuffers() noexcept
{
    Extent2D extent = m_pSwapchain->getExtent();

    // Create the depth image
    m_depthImage = Image{
        *m_pRenderDevice, 
        VK_IMAGE_TYPE_2D, 
        m_depthFormat, 
        extent, 
        1, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT};

    /* The previous objects must be destroyed before this point. */
    m_swapImageViews.clear();
    m_swapFramebuffers.clear();

    auto& images = m_pSwapchain->getImages();

    for (VkImage image : images)
    {
        const VkImageViewCreateInfo imageViewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_pSwapchain->getFormat(),
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
        };

        VkImageView imageView = {};
        if (vkCreateImageView(m_pRenderDevice->getDevice(), &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            Logger::Error("Could not create a vulkan image view for the renderer!\n");
            goto failureCleanup;
        }

        const std::array<VkImageView, 2> attachments = {
            imageView,
            m_depthImage.getImageView()
        };

        const VkFramebufferCreateInfo framebufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_pass,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .width = (uint32_t)extent.width,
            .height = (uint32_t)extent.height,
            .layers = 1,
        };

        VkFramebuffer framebuffer = {};
        if (vkCreateFramebuffer(m_pRenderDevice->getDevice(), &framebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            Logger::Error("Could not create a vulkan framebuffer for the renderer!");
            goto failureCleanup;
        }

        m_swapImageViews.push_back(imageView);
        m_swapFramebuffers.push_back(framebuffer);
    }

    return true;

failureCleanup:
    for (VkImageView view : m_swapImageViews)
        vkDestroyImageView(m_pRenderDevice->getDevice(), view, nullptr);
    
    for (VkFramebuffer fb : m_swapFramebuffers)
        vkDestroyFramebuffer(m_pRenderDevice->getDevice(), fb, nullptr);

    return false;

}

bool Renderer::createSyncObjects() noexcept
{
    const VkSemaphoreCreateInfo semaphoreCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    const VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    m_imageAvailableSemaphores.resize(DEFAULT_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(DEFAULT_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(DEFAULT_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < DEFAULT_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(m_pRenderDevice->getDevice(), &semaphoreCreateInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_pRenderDevice->getDevice(), &semaphoreCreateInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_pRenderDevice->getDevice(), &fenceCreateInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            Logger::Error("Could not create a vulkan frame semaphore!\n");
            goto failureCleanup;
        }
    }

    return true;

failureCleanup:

    for (uint32_t i = 0; i < DEFAULT_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_pRenderDevice->getDevice(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_pRenderDevice->getDevice(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_pRenderDevice->getDevice(), m_inFlightFences[i], nullptr);
    }

    return false;
}

bool Renderer::recreateSwappable() noexcept
{   
    destroySwappable();

    return createFrameBuffers() 
        && createCommandBuffers() 
        && createSyncObjects();
}

void Renderer::destroySwappable() noexcept
{
    if (!m_pRenderDevice) return;

    // The device must be idle before we destroy anything and command buffers must stop.
    vkDeviceWaitIdle(m_pRenderDevice->getDevice());

    if (m_swapCommandBuffers.size() > 0)
        vkFreeCommandBuffers(m_pRenderDevice->getDevice(), m_pRenderDevice->getCommandPool(), m_pSwapchain->getImageCount(), m_swapCommandBuffers.data());

    for (uint32_t i = 0; i < m_swapFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(m_pRenderDevice->getDevice(), m_swapFramebuffers[i], nullptr);
        vkDestroyImageView(m_pRenderDevice->getDevice(), m_swapImageViews[i], nullptr);
    }

    for (uint32_t i = 0; i < m_imageAvailableSemaphores.size(); i++)
    {
        vkDestroySemaphore(m_pRenderDevice->getDevice(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_pRenderDevice->getDevice(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_pRenderDevice->getDevice(), m_inFlightFences[i], nullptr);
    }
}

void Renderer::collapse() noexcept
{
    destroySwappable();

    if (m_pass) 
        vkDestroyRenderPass(m_pRenderDevice->getDevice(), m_pass, nullptr);

    m_pRenderDevice = nullptr;
    m_pSwapchain = nullptr;
    m_depthFormat = VK_FORMAT_UNDEFINED;
    m_pass = VK_NULL_HANDLE;
    m_currentFrame = 0;
    m_previousFrame = 0;
    m_imageIndex = 0;
    m_framebufferResized = false;
    m_deadFrame = false;
    m_swapCommandBuffers.clear();
    m_swapImageViews.clear();
    m_swapFramebuffers.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_inFlightFences.clear();
}

} // namespace bl