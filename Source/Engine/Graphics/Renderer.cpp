#include "Core/Print.h"
#include "VulkanDescriptorSetCache.h"
#include "Renderer.h"
#include <exception>

namespace bl {

Renderer::Renderer(VulkanDevice* device, VulkanWindow* window)
    : _device(device)
    , _window(window)
    , _swapchain(window->GetSwapchain())
    , _currentFrame(0)
    , _descriptorSetCache(_device, 1024, VulkanDescriptorRatio::Default()) {
    _commandBuffers.resize(VulkanConfig::numFramesInFlight);
    _imageAvailableSemaphores.resize(_swapchain->GetImageCount());
    _renderFinishedSemaphores.resize(_swapchain->GetImageCount());
    _inFlightFences.resize(_swapchain->GetImageCount());

    try {
        CreateSyncObjects();
        CreateRenderPasses();
        RecreateImages();
    } catch (const std::exception& e) {
        DestroyImagesAndFramebuffers();
        DestroyRenderPasses();
        DestroySyncObjects();
        throw;
    }
}

Renderer::~Renderer()  {
    _device->WaitForDevice();

    DestroyImagesAndFramebuffers();
    DestroyRenderPasses();
    DestroySyncObjects();
}

VkRenderPass Renderer::GetRenderPass() const { 
    return _pass;
}

void Renderer::CreateSyncObjects() {
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = VulkanConfig::numFramesInFlight;

    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, _commandBuffers.data()))

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < VulkanConfig::numFramesInFlight; i++) {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]))
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &_inFlightFences[i]))
    }
}

void Renderer::DestroySyncObjects() {
    for (uint32_t i = 0; i < VulkanConfig::numFramesInFlight; i++) {
        vkDestroySemaphore(_device->Get(), _imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(_device->Get(), _renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(_device->Get(), _inFlightFences[i], nullptr);
    }

    vkFreeCommandBuffers(_device->Get(), _device->GetCommandPool(), (uint32_t)_commandBuffers.size(), _commandBuffers.data());
}

void Renderer::DestroyImagesAndFramebuffers() {
    for (VkFramebuffer fb : _framebuffers)
        vkDestroyFramebuffer(_device->Get(), fb, nullptr);

    _framebuffers.clear();
    _depthImages.clear();
}

void Renderer::RecreateImages() {
    _imageCount = _swapchain->GetImageCount();
    VkExtent2D extent = _swapchain->GetExtent();
    
    DestroyImagesAndFramebuffers();

    // Construct all the buffers for the passes.
    _depthImages.reserve(_imageCount);
    auto imageExtent = VkExtent3D{extent.width, extent.height, 1};

    for (uint32_t i = 0; i < _imageCount; i++) {
        _depthImages.emplace_back(_device, VK_IMAGE_TYPE_2D, imageExtent, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    _framebuffers.reserve(_imageCount);

    auto swapchainImageViews = _swapchain->GetImageViews();
    for (uint32_t i = 0; i < _imageCount; i++) {
        std::array attachments = {
            swapchainImageViews[i],
            _depthImages[i].GetView()
        };

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = _pass;
        createInfo.attachmentCount = (uint32_t)attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = extent.width;
        createInfo.height = extent.height;
        createInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(_device->Get(), &createInfo, nullptr, &_framebuffers[i]))
    }
}

void Renderer::Render(std::function<void(VkCommandBuffer, uint32_t)> func)
{
    if (!_swapchain->Get()) // Swapchain must be valid.
        return;

    // Wait for the current image up coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->Get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX))

    // Acquire the next image in the swapchain and update all render pass
    // images if the swapchain was recreated within the previous frame.
    if (_swapchain->AcquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE)) {
        RecreateImages();
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

    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo))
    
    std::array clearColors = {
        VkClearValue{ .color = {0.96f, 0.97f, 0.96f, 1.0f}}, // Swapchain Image Clear Color
        VkClearValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f}}, // Position Clear Color
        VkClearValue{ .color = {0.0f, 0.0f, 0.0f, 1.0f}}, // Albedo Clear Color
        VkClearValue{ .color = {0.0f, 0.0f, 0.0f, 0.0f}} // Normal/Specular Clear Color
    };

    VkRect2D renderArea{ { 0, 0 }, _swapchain->GetExtent() };

    VkRenderPassBeginInfo passBeginInfo = {};
    passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    passBeginInfo.pNext = nullptr;
    passBeginInfo.renderPass = _pass;
    passBeginInfo.framebuffer = _framebuffers[imageIndex];
    passBeginInfo.renderArea = renderArea;
    passBeginInfo.clearValueCount = (uint32_t)clearColors.size();
    passBeginInfo.pClearValues = clearColors.data();
    
    vkCmdBeginRenderPass(cmd, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Render all the frame data to the gbuffer.
    func(cmd, _currentFrame);
    
    vkCmdEndRenderPass(cmd);

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

    if (_swapchain->QueuePresent(_renderFinishedSemaphores[_currentFrame])) {
        RecreateImages();
    }

    _currentFrame = (_currentFrame + 1) % VulkanConfig::numFramesInFlight;
}

void Renderer::CreateRenderPasses() {

    // Find the formats for each image in the pass.
    auto physicalDevice = _device->GetPhysicalDevice();
    _depthFormat = physicalDevice->FindSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, 0);

    // Build the renderpasses attachment data.
    std::array<VkAttachmentDescription, 2> attachments = {};
    
    // Swapchain Present Attachment (Final Image)
    attachments[0].flags = 0;
    attachments[0].format = _swapchain->GetFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Depth Attachment
    attachments[1].flags = 0;
    attachments[1].format = _depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // stencil may be used later
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    std::array<VkSubpassDescription, 1> subpasses;

    // Forward Subpass
    std::array<VkAttachmentReference, 1> forwardColorReferences = {{
        {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
    }};

    VkAttachmentReference forwardDepthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL 
    };

    subpasses[0].flags = {};
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = (uint32_t)forwardColorReferences.size();
    subpasses[0].pColorAttachments = forwardColorReferences.data();
    subpasses[0].pResolveAttachments = nullptr;
    subpasses[0].pDepthStencilAttachment = &forwardDepthReference;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;

    std::array<VkSubpassDependency, 1> dependencies = {};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = 0;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = (uint32_t)attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = (uint32_t)subpasses.size();
    createInfo.pSubpasses = subpasses.data();
    createInfo.dependencyCount = (uint32_t)dependencies.size();
    createInfo.pDependencies = dependencies.data();

    VK_CHECK(vkCreateRenderPass(_device->Get(), &createInfo, nullptr, &_pass))
}

void Renderer::DestroyRenderPasses() {
    vkDestroyRenderPass(_device->Get(), _pass, nullptr);
}

VulkanDescriptorSetCache* Renderer::GetDescriptorSetCache() {
    return &_descriptorSetCache;
}

} // namespace bl
