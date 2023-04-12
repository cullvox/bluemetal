#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Renderer.hpp"

#include <array>

namespace bl 
{

Renderer::Renderer() noexcept
    : _pRenderDevice(nullptr)
    , _pSwapchain(nullptr)
    , _pass(VK_NULL_HANDLE)
    , _currentFrame(0)
    , _previousFrame(0)
    , _imageIndex(0)
    , _framebufferResized(false)
    , _deadFrame(false)
    , _swapCommandBuffers({})
    , _swapImageViews({})
    , _swapFramebuffers({})
    , _imageAvailableSemaphores({})
    , _renderFinishedSemaphores({})
    , _inFlightFences({})
{
}

Renderer::Renderer(RenderDevice& renderDevice, Swapchain& swapchain) noexcept
    : Renderer()
{
    _pRenderDevice = &renderDevice;
    _pSwapchain = &swapchain;

    if (not _pRenderDevice->good() ||
        not _pSwapchain->good())
    {
        Logger::Error("Cannot construct the renderer parameter was invalid!");
        return;
    }

    /* Create the swapchain based objects and the render pass. */
    createRenderPass();
    recreate();
}

Renderer::~Renderer()
{
    collapse();
}

Renderer& Renderer::operator=(Renderer&& rhs) noexcept
{
    collapse();

    _pRenderDevice = std::move(rhs._pRenderDevice);
    _pSwapchain = std::move(rhs._pSwapchain);
    _depthFormat = rhs._depthFormat;
    _depthImage = std::move(rhs._depthImage);
    _pass = rhs._pass;
    _currentFrame = rhs._currentFrame;
    _previousFrame = rhs._previousFrame;
    _imageIndex = rhs._imageIndex;
    _framebufferResized = rhs._framebufferResized;
    _deadFrame = rhs._deadFrame;
    _swapCommandBuffers = rhs._swapCommandBuffers;
    _swapImageViews = rhs._swapImageViews;
    _swapFramebuffers = rhs._swapFramebuffers;
    _imageAvailableSemaphores = rhs._imageAvailableSemaphores;
    _renderFinishedSemaphores = rhs._renderFinishedSemaphores;
    _inFlightFences = rhs._inFlightFences;

    rhs._pRenderDevice = nullptr;
    rhs._pSwapchain = nullptr;
    rhs._depthFormat = VK_FORMAT_UNDEFINED;
    rhs._pass = VK_NULL_HANDLE;
    rhs._currentFrame = 0;
    rhs._previousFrame = 0;
    rhs._imageIndex = 0;
    rhs._framebufferResized = false;
    rhs._deadFrame = false;
    rhs._swapCommandBuffers.clear();
    rhs._swapImageViews.clear();
    rhs._swapFramebuffers.clear();
    rhs._imageAvailableSemaphores.clear();
    rhs._renderFinishedSemaphores.clear();
    rhs._inFlightFences.clear();

    return *this;
}

void Renderer::submit(const Submission& submission) noexcept
{
    //vkCmdBindPipeline(_swapCommandBuffers[_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, submission.pipeline);
}

bool Renderer::good() const noexcept
{
    return (_pRenderDevice->good() &&
        _depthImage.good() &&
        _swapCommandBuffers.size() > 0 &&
        _swapFramebuffers.size() > 0 &&
        _swapImageViews.size() > 0 &&
        _imageAvailableSemaphores.size() > 0 &&
        _renderFinishedSemaphores.size() > 0 &&
        _inFlightFences.size() > 0);
}

bool Renderer::beginFrame(VkCommandBuffer& commandBuffer) noexcept
{

    // If the swapchain wasn't recreated for some reason skip the frame.
    if (!_pSwapchain->good())
    {
        _deadFrame = true;
        return true;
    }

    vkWaitForFences(_pRenderDevice->getDevice(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire the next image from the swapchain.
    bool requiresRecreation = false;
    BL_CHECK(
        _pSwapchain->acquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, _imageIndex, requiresRecreation),
        "Could not acquire the swapchains next image!\n")

    // Attempt to recreate the swapchain and if it fails we need to 
    if (requiresRecreation)
    {
        recreate();

        _deadFrame = true;
        return false;
    }

    vkResetFences(_pRenderDevice->getDevice(), 1, &_inFlightFences[_currentFrame]);

    // Reset the prevous frames command buffer.
    vkResetCommandBuffer(_swapCommandBuffers[_currentFrame], 0);

    // Begin the command buffer for this frame, submissions are open.
    const VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };

    BL_CHECK(
        vkBeginCommandBuffer(_swapCommandBuffers[_currentFrame], &beginInfo) == VK_SUCCESS,
        "Could not begin a command buffer after this frame!\n")

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

    const Extent2D extent = _pSwapchain->getExtent();
    const VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = _pass,
        .framebuffer = _swapFramebuffers[_imageIndex],
        .renderArea = { 
            {0, 0}, // offset
            {(uint32_t)extent.width, (uint32_t)extent.height}, //extent
        },
        .clearValueCount = (uint32_t)clearValues.size(),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass(_swapCommandBuffers[_currentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    commandBuffer = _swapCommandBuffers[_currentFrame];

    return true;
}

bool Renderer::endFrame() noexcept
{
    // If this frame is considered dead don't do anything and goto next frame.
    if (_deadFrame)
    {
        _deadFrame = false;
        return true;
    }

    // End the previous command buffer.
    vkCmdEndRenderPass(_swapCommandBuffers[_currentFrame]);
    vkEndCommandBuffer(_swapCommandBuffers[_currentFrame]);

    // Submit the command buffer for rendering.
    const VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    const VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &_swapCommandBuffers[_currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    BL_CHECK(
        vkQueueSubmit(_pRenderDevice->getGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) == VK_SUCCESS,
        "Could not submit a draw command buffer!\n")

    VkResult result = {};

    // Present the rendered image.
    const VkSwapchainKHR swapChains[] = {_pSwapchain->getSwapchain()};
    const VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &_imageIndex,
        .pResults = nullptr,
    };

    result = vkQueuePresentKHR(_pRenderDevice->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized)
    {
        // Recreate the swapchain next frame.
    }
    else if (result != VK_SUCCESS)
    {
        Logger::Error("Could not queue the vulkan present!");
        return false;
    }

    // Move the frame number to the next in sequence.
    _currentFrame = (_currentFrame + 1) % DEFAULT_FRAMES_IN_FLIGHT;

    return true;
}

bool Renderer::createRenderPass() noexcept
{
    _depthFormat = _pRenderDevice->findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    const std::array<VkAttachmentDescription, 2> attachmentDescriptions = {{
        { // Color Attachment
            0, // flags
            _pSwapchain->getFormat(), // format
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
            _depthFormat, // format
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

    BL_CHECK(
        vkCreateRenderPass(_pRenderDevice->getDevice(), &createInfo, nullptr, &_pass) == VK_SUCCESS,
        "Could not create a vulkan render pass!\n")

    return true;
}

VkRenderPass Renderer::getRenderPass() const noexcept
{
    return _pass;
}

bool Renderer::createCommandBuffers() noexcept
{
    const VkCommandBufferAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = _pRenderDevice->getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = _pSwapchain->getImageCount(),
    };

    _swapCommandBuffers.resize(_pSwapchain->getImageCount());

    BL_CHECK(
        vkAllocateCommandBuffers(_pRenderDevice->getDevice(), &allocateInfo, _swapCommandBuffers.data()) == VK_SUCCESS,
        "Could not allocate vulkan swapchain command buffers!\n")

    return true;
}

bool Renderer::createFrameBuffers() noexcept
{
    Extent2D extent = _pSwapchain->getExtent();

    // Create the depth image
    _depthImage = Image{
        *_pRenderDevice, 
        VK_IMAGE_TYPE_2D, 
        _depthFormat, 
        extent, 
        1, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT};

    /* The previous objects must be destroyed before this point. */
    _swapImageViews.clear();
    _swapFramebuffers.clear();

    auto& images = _pSwapchain->getImages();

    for (VkImage image : images)
    {
        const VkImageViewCreateInfo imageViewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = _pSwapchain->getFormat(),
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
        BL_CHECK_GOTO(
            vkCreateImageView(_pRenderDevice->getDevice(), &imageViewCreateInfo, nullptr, &imageView) == VK_SUCCESS,
            "Could not create a vulkan image view for the renderer!\n",
            failureCleanup)

        const std::array<VkImageView, 2> attachments = {
            imageView,
            _depthImage.getImageView()
        };

        const VkFramebufferCreateInfo framebufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = _pass,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .width = (uint32_t)extent.width,
            .height = (uint32_t)extent.height,
            .layers = 1,
        };

        VkFramebuffer framebuffer = {};
        BL_CHECK_GOTO(
            vkCreateFramebuffer(_pRenderDevice->getDevice(), &framebufferCreateInfo, nullptr, &framebuffer) == VK_SUCCESS,
            "Could not create a vulkan framebuffer for the renderer!\n",
            failureCleanup)

        _swapImageViews.push_back(imageView);
        _swapFramebuffers.push_back(framebuffer);
    }

    return true;

failureCleanup:
    for (VkImageView view : _swapImageViews)
        vkDestroyImageView(_pRenderDevice->getDevice(), view, nullptr);
    
    for (VkFramebuffer fb : _swapFramebuffers)
        vkDestroyFramebuffer(_pRenderDevice->getDevice(), fb, nullptr);

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

    _imageAvailableSemaphores.resize(DEFAULT_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(DEFAULT_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(DEFAULT_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < DEFAULT_FRAMES_IN_FLIGHT; i++)
    {
        BL_CHECK_GOTO(
                vkCreateSemaphore(_pRenderDevice->getDevice(), &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphores[i]) == VK_SUCCESS &&
                vkCreateSemaphore(_pRenderDevice->getDevice(), &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphores[i]) == VK_SUCCESS &&
                vkCreateFence(_pRenderDevice->getDevice(), &fenceCreateInfo, nullptr, &_inFlightFences[i]) == VK_SUCCESS,
            "Could not create a vulkan frame semaphore!\n",
            failureCleanup)
    }

    return true;

failureCleanup:
    for (uint32_t i = 0; i < DEFAULT_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(_pRenderDevice->getDevice(), _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_pRenderDevice->getDevice(), _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(_pRenderDevice->getDevice(), _inFlightFences[i], nullptr);
    }

    return false;
}

bool Renderer::recreate() noexcept
{   
    BL_CHECK_NL(_pSwapchain->recreate())

    destroySwappable();

    return createFrameBuffers() 
        && createCommandBuffers() 
        && createSyncObjects();
}

void Renderer::destroySwappable() noexcept
{
    if (!_pRenderDevice) return;

    // The device must be idle before we destroy anything and command buffers must stop.
    vkDeviceWaitIdle(_pRenderDevice->getDevice());

    if (_swapCommandBuffers.size() > 0)
        vkFreeCommandBuffers(_pRenderDevice->getDevice(), _pRenderDevice->getCommandPool(), _pSwapchain->getImageCount(), _swapCommandBuffers.data());

    for (uint32_t i = 0; i < _swapFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(_pRenderDevice->getDevice(), _swapFramebuffers[i], nullptr);
        vkDestroyImageView(_pRenderDevice->getDevice(), _swapImageViews[i], nullptr);
    }

    for (uint32_t i = 0; i < _imageAvailableSemaphores.size(); i++)
    {
        vkDestroySemaphore(_pRenderDevice->getDevice(), _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_pRenderDevice->getDevice(), _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(_pRenderDevice->getDevice(), _inFlightFences[i], nullptr);
    }
}

void Renderer::collapse() noexcept
{
    destroySwappable();

    if (_pass) 
        vkDestroyRenderPass(_pRenderDevice->getDevice(), _pass, nullptr);

    _pRenderDevice = nullptr;
    _pSwapchain = nullptr;
    _depthFormat = VK_FORMAT_UNDEFINED;
    _pass = VK_NULL_HANDLE;
    _currentFrame = 0;
    _previousFrame = 0;
    _imageIndex = 0;
    _framebufferResized = false;
    _deadFrame = false;
    _swapCommandBuffers.clear();
    _swapImageViews.clear();
    _swapFramebuffers.clear();
    _imageAvailableSemaphores.clear();
    _renderFinishedSemaphores.clear();
    _inFlightFences.clear();
}

} // namespace bl