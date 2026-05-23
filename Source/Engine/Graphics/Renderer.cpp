#include "Renderer.h"
#include "Core/FrameCounter.h"
#include "Core/Profiler.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "GraphicsSystem.h"
#include "Resources/Mesh.h"
#include "Scene/Node.h"
#include "UniformData.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanMaterial.h"
#include "VulkanWindow.h"

namespace bl {

Renderer::Renderer(VulkanWindow* window, FrameCounter& frameCounter)
    : _device(window->GetDevice())
    , _window(window)
    , _swapchain(window->GetSwapchain())
    , _frameCounter(frameCounter)
    , _renderData(this)
{

    for (VkSampleCountFlagBits flag : GetMultisampleCounts()) {
        if (flag & VK_SAMPLE_COUNT_8_BIT)
            _sampleCount = VK_SAMPLE_COUNT_8_BIT;
        if (flag & VK_SAMPLE_COUNT_4_BIT)
            _sampleCount = VK_SAMPLE_COUNT_4_BIT;
        if (flag & VK_SAMPLE_COUNT_2_BIT)
            _sampleCount = VK_SAMPLE_COUNT_2_BIT;
    }

    try {
        _descriptorSetCache = std::make_unique<VulkanDescriptorSetAllocatorCache>(_device, 1024, VulkanDescriptorRatio::Default());

        auto physicalDevice = _device->GetPhysicalDevice();
        _depthFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, 0);
        _positionFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_R32G32B32A32_SFLOAT }, VK_IMAGE_TILING_OPTIMAL, 0);

        CreatePerFrameSyncedData();
        RecreateImages();
        CreateGlobalUniform();

    } catch (const std::exception& e) {
        Print::Error("Failed to initialize renderer: {}", e.what());
        DestroyGlobalUniform();
        DestroyImagesAndFramebuffers();
        DestroyPerFrameSyncedData();
        throw e;
    }
}

Renderer::~Renderer()
{
    _device->WaitForDevice();

    DestroyGlobalUniform();
    DestroyImagesAndFramebuffers();
    DestroyPerFrameSyncedData();
}

void Renderer::SetProjection(const glm::mat4& projection)
{
    _uboData.projection = projection;
    _renderData.SetProjectionMatrix(projection);
}

void Renderer::SetView(const glm::mat4& view)
{
    _uboData.view = view;
    _renderData.SetViewMatrix(view);
}

void Renderer::CreatePerFrameSyncedData()
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = VulkanConfig::maxFramesInFlight;

    VkCommandBuffer commandBuffers[VulkanConfig::maxFramesInFlight];
    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, commandBuffers))

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) 
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_perFrame[i].imageAvailableSemaphore))
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &_perFrame[i].inFlightFence))
        _perFrame[i].commandBuffer = commandBuffers[i];
    }

    _renderFinishedSemaphores.resize(_swapchain->GetImageCount());
    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++) 
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
    }
}

void Renderer::DestroyPerFrameSyncedData()
{
    VkCommandBuffer commandBuffers[VulkanConfig::maxFramesInFlight];
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        vkDestroySemaphore(_device->Get(), _perFrame[i].imageAvailableSemaphore, nullptr);
        vkDestroyFence(_device->Get(), _perFrame[i].inFlightFence, nullptr);
        commandBuffers[i] = _perFrame[i].commandBuffer;
    }

    vkFreeCommandBuffers(_device->Get(), _device->GetCommandPool(), VulkanConfig::maxFramesInFlight, commandBuffers);

    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++)
    {
        vkDestroySemaphore(_device->Get(), _renderFinishedSemaphores[i], nullptr);
    }
}

void Renderer::DestroyImagesAndFramebuffers()
{
    _colorImage.reset();
    _depthImage.reset();
}

void Renderer::DestroyGlobalUniform()
{
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _descriptorSetCache->Free(_globalDescriptorLayout, _globalDescriptorSets[i]);
    }
}

VulkanDevice* Renderer::GetDevice() const
{
    return _device;
}

uint32_t Renderer::GetSwapchainImageCount()
{
    return _swapchain->GetImageCount();
}

void Renderer::RecreateImages()
{
    DestroyImagesAndFramebuffers();

    // Construct all the image buffers for the passes.
    auto extent = _swapchain->GetExtent();
    auto imageExtent = VkExtent3D { extent.width, extent.height, 1 };

    // Create color image
    VkComponentMapping mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    _colorImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _swapchain->GetFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, false, _sampleCount);
    _colorImageView = std::make_unique<VulkanImageView>(_device, _colorImage.get(), VK_IMAGE_VIEW_TYPE_2D, _swapchain->GetFormat(), mapping, range);

    // Create selection buffer images
    _selectionImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, VK_FORMAT_R32_UINT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    _selectionImageView = std::make_unique<VulkanImageView>(_device, _selectionImage.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);

    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) 
    {
        _selectionImageSampled = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, VK_FORMAT_R32_UINT, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, false, _sampleCount);
        _selectionImageSampledView = std::make_unique<VulkanImageView>(_device, _selectionImageSampled.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);
    }

    _selectionBuffer = std::make_unique<VulkanBuffer>(_device, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU, extent.width * extent.height * sizeof(uint32_t), nullptr, true, VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Create depth image
    _depthImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false, _sampleCount);
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    _depthImageView = std::make_unique<VulkanImageView>(_device, _depthImage.get(), VK_IMAGE_VIEW_TYPE_2D, _depthFormat, mapping, range);

    // Transition depth image
    _device->ImmediateSubmit([&](VkCommandBuffer cmd){

        std::array<VkImageMemoryBarrier2, 4> barriers = {};
        barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[0].pNext = nullptr;
        barriers[0].srcStageMask = 0;
        barriers[0].srcAccessMask = 0;
        barriers[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        barriers[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[0].newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[0].image = _depthImage->Get();
        barriers[0].subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
    
        barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[1].pNext = nullptr;
        barriers[1].srcStageMask = 0;
        barriers[1].srcAccessMask = 0;
        barriers[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].image = _colorImage->Get();
        barriers[1].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        barriers[2].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[2].pNext = nullptr;
        barriers[2].srcStageMask = 0;
        barriers[2].srcAccessMask = 0;
        barriers[2].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[2].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[2].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[2].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[2].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[2].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[2].image = _selectionImage->Get();
        barriers[2].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        barriers[3].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[3].pNext = nullptr;
        barriers[3].srcStageMask = 0;
        barriers[3].srcAccessMask = 0;
        barriers[3].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[3].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[3].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[3].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[3].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[3].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[3].image = _selectionImageSampled->Get();
        barriers[3].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        VkDependencyInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        info.pNext = nullptr,
        info.dependencyFlags = 0,
        info.imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
        info.pImageMemoryBarriers = barriers.data();

        vkCmdPipelineBarrier2(cmd, &info);
    });


    // Create image views for swapchain images
    _swapchainImages = _swapchain->GetImages();
    _swapchainImageViews = _swapchain->GetImageViews();
}

VkPipelineStageFlags getPipelineStageFlags(VkImageLayout layout)
{
    switch (layout) {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        return VK_PIPELINE_STAGE_HOST_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        return VK_PIPELINE_STAGE_TRANSFER_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
        return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    case VK_IMAGE_LAYOUT_GENERAL:
        assert(false && "Don't know how to get a meaningful VkPipelineStageFlags for VK_IMAGE_LAYOUT_GENERAL! Don't use it!");
        return 0;
    default:
        assert(false);
        return 0;
    }
}

VkAccessFlags getAccessFlags(VkImageLayout layout)
{
    switch (layout) {
    case VK_IMAGE_LAYOUT_UNDEFINED:
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        return 0;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        return VK_ACCESS_HOST_WRITE_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
        return VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        return VK_ACCESS_TRANSFER_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        return VK_ACCESS_TRANSFER_WRITE_BIT;
    case VK_IMAGE_LAYOUT_GENERAL:
        assert(false && "Don't know how to get a meaningful VkAccessFlags for VK_IMAGE_LAYOUT_GENERAL! Don't use it!");
        return 0;
    default:
        assert(false);
        return 0;
    }
}

Profiler profiler;

void Renderer::Render(Node*)
{
}

void Renderer::Render(RenderFunction func, RenderFunction guiPassFunc,  ObjectFunction objectFunc)
{
    // If the window is minimized, we don't draw anything.
    if (_window->GetMinimized())
        return;

    _renderData.SetCurrentFrame(_currentFrame);

    PerFrameData& frame = _perFrame[_currentFrame];

    // Wait for the current image up coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->Get(), 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX))
    VK_CHECK(vkResetFences(_device->Get(), 1, &frame.inFlightFence))

    if (recreateRequested) {
        _device->WaitForDevice(); // Wait for previous commands to complete.

        // DestroyRenderPasses();
        // CreateRenderPasses();
        if (_changedSampleCount)
        {
            _sampleCount = _newSampleCount;
            _changedSampleCount = false;
        }
        _swapchain->Recreate(recreatePresentMode);
        RecreateImages();
        recreateRequested = false;
        _device->WaitForDevice();
    }

    // Prepare uniform buffers for the next frame.
    UpdateMaterialUniforms();

    // Compute the per frame UBO.
    UpdateGlobalUniform();

    // Swapchain must be valid.
    if (!_swapchain->Get()) {
        return;
    }

    // Acquire the next image in the swapchain and update all render pass
    // images if the swapchain was recreated within the previous frame.
    uint32_t imageIndex = 0;
    if (_swapchain->AcquireNext(imageIndex, frame.imageAvailableSemaphore)) {
        RecreateImages();
        return; // skip this frame!
    }

    _renderData.SetImageIndex(imageIndex);

    auto cmd = frame.commandBuffer;
    VK_CHECK(vkResetCommandBuffer(cmd, 0))

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo))

    _renderData.SetCommandBuffer(cmd);
    _renderData.SetSampleCount(_sampleCount);
        // Render all the frame data to the gbuffer.
    _renderData.SetGlobalDescriptorSet(_globalDescriptorSets[_currentFrame]);

    // This function doesn't know about globals or uniforms yet.

    objectFunc(_renderData);

    _renderData.WriteInstanceBuffer();

    // Setup the render pass for dynamic rendering.
    std::array clearColors = {
        VkClearValue { .color = { { 0.96f, 0.97f, 0.96f, 1.0f } } }, // Clear Color
        VkClearValue { .depthStencil = { 1.0f, 0 } }, // Clear Depth
        VkClearValue { .color = { -1, -1, -1, -1 } }
    };

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = _swapchain->GetExtent();

    std::array<VkRenderingAttachmentInfo, 2> colorAttachments = {};
    colorAttachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[0].pNext = nullptr;
    colorAttachments[0].imageView = _swapchainImageViews[imageIndex];
    colorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[0].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[0].clearValue = clearColors[0];

    // When using a higher sample count, the image must be resolved from the sampled image.
    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachments[0].imageView = _colorImageView->Get();
        colorAttachments[0].resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[0].resolveImageView = _swapchainImageViews[imageIndex];
    }

    colorAttachments[1].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[1].pNext = nullptr;
    colorAttachments[1].imageView = _selectionImageView->Get();
    colorAttachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[1].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[1].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[1].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[1].clearValue = VkClearValue { .color = { -1, -1, -1, -1 } };

    // When using a higher sample count, the image must be resolved from the sampled image.
    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachments[1].imageView = _selectionImageSampledView->Get();
        colorAttachments[1].resolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
        colorAttachments[1].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[1].resolveImageView = _selectionImageView->Get();
    }

    VkRenderingAttachmentInfo depthAttachment = {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.pNext = nullptr;
    depthAttachment.imageView = _depthImageView->Get();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
    depthAttachment.resolveImageView = VK_NULL_HANDLE;
    depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue = clearColors[1];

    VkRenderingInfo renderingInfo = {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.pNext = nullptr;
    renderingInfo.flags = 0;
    renderingInfo.renderArea = renderArea;
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
    renderingInfo.pColorAttachments = colorAttachments.data();
    renderingInfo.pDepthAttachment = &depthAttachment;
    renderingInfo.pStencilAttachment = nullptr;

    VkImageSubresourceRange range = {};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    uint32_t barrierCount = 1;
    std::array<VkImageMemoryBarrier2, 4> barriers = {};

    // Transition the swapchain image back into a color attachment.
    barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barriers[0].pNext = nullptr;
    barriers[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    barriers[0].srcAccessMask = 0;
    barriers[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    barriers[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barriers[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].image = _swapchainImages[imageIndex];
    barriers[0].subresourceRange = range;

    VkDependencyInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;
    info.dependencyFlags = 0;
    info.imageMemoryBarrierCount = 1;
    info.pImageMemoryBarriers = barriers.data();

    vkCmdPipelineBarrier2(cmd, &info);

    vkCmdBeginRendering(cmd, &renderingInfo);

    // Render all the frame data to the gbuffer.
    _renderData.SetGlobalDescriptorSet(_globalDescriptorSets[_currentFrame]);

    VkExtent2D extent = _swapchain->GetExtent();

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewportWithCount(cmd, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { extent.width, extent.height };
    vkCmdSetScissorWithCount(cmd, 1, &scissor);

    _renderData.SetSampleCount(_sampleCount);

    _renderData.WriteDrawCommands();

    func(_renderData);

    vkCmdEndRendering(cmd);

    colorAttachments[0].imageView = _swapchainImageViews[imageIndex];
    colorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[0].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[0].clearValue = clearColors[0];

    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = colorAttachments.data();
    renderingInfo.pDepthAttachment = nullptr;

    vkCmdBeginRendering(cmd, &renderingInfo);

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    _renderData.SetSampleCount(VK_SAMPLE_COUNT_1_BIT);
    guiPassFunc(_renderData);

    vkCmdEndRendering(cmd);

    // Barrier transition the swapchain image to a presentable layout.
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    barrierCount = 1;

    barriers[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    barriers[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barriers[0].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    barriers[0].dstAccessMask = 0;
    barriers[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barriers[0].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].image = _swapchainImages[imageIndex];
    barriers[0].subresourceRange = range;

    if (_queuedSelectionBuffer)
    {
        barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[1].pNext = nullptr;
        barriers[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[1].dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        barriers[1].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barriers[1].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].image = _selectionImage->Get();
        barriers[1].subresourceRange = range;

        barrierCount++;
    }

    info.memoryBarrierCount = 0;
    info.bufferMemoryBarrierCount = 0;
    info.imageMemoryBarrierCount = barrierCount;
    info.pImageMemoryBarriers = &barriers[0];

    vkCmdPipelineBarrier2(cmd, &info);

    if (_queuedSelectionBuffer)
    {
        VkImageSubresourceLayers layers = {};
        layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        layers.mipLevel = 0;
        layers.baseArrayLayer = 0;
        layers.layerCount = 1;

        VkBufferImageCopy2 copy = {};
        copy.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
        copy.pNext = nullptr;
        copy.bufferOffset = 0;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource = layers;
        copy.imageOffset = {};
        copy.imageExtent = VkExtent3D{extent.width, extent.height, 1};

        VkCopyImageToBufferInfo2 selectionCopy = {};
        selectionCopy.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
        selectionCopy.pNext = nullptr;
        selectionCopy.srcImage = _selectionImage->Get();
        selectionCopy.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        selectionCopy.dstBuffer = _selectionBuffer->Get();
        selectionCopy.regionCount = 1;
        selectionCopy.pRegions = &copy;

        vkCmdCopyImageToBuffer2(cmd, &selectionCopy);

        barriers[0].srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barriers[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barriers[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[0].image = _selectionImage->Get();
        barriers[0].subresourceRange = range;

        info.memoryBarrierCount = 0;
        info.bufferMemoryBarrierCount = 0;
        info.imageMemoryBarrierCount = 1;
        info.pImageMemoryBarriers = &barriers[0];

        vkCmdPipelineBarrier2(cmd, &info);
        _queuedSelectionBuffer = false;
    }

    VK_CHECK(vkEndCommandBuffer(cmd))

    // Submit the command buffer to the graphics queue.
    VkSemaphoreSubmitInfo waitSemaphoreInfo = {};
    waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSemaphoreInfo.pNext = nullptr;
    waitSemaphoreInfo.semaphore = frame.imageAvailableSemaphore;
    waitSemaphoreInfo.value = 0;
    waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    waitSemaphoreInfo.deviceIndex = 0;

    VkCommandBufferSubmitInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferInfo.pNext = nullptr;
    commandBufferInfo.commandBuffer = cmd;
    commandBufferInfo.deviceMask = 0;

    VkSemaphoreSubmitInfo signalSemaphoreInfo = {};
    signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalSemaphoreInfo.pNext = nullptr;
    signalSemaphoreInfo.semaphore = _renderFinishedSemaphores[imageIndex];
    signalSemaphoreInfo.value = 0;
    signalSemaphoreInfo.stageMask = 0;
    signalSemaphoreInfo.deviceIndex = 0;

    VkSubmitInfo2 submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.pNext = nullptr;
    submitInfo.flags = 0;
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferInfo;
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

    VK_CHECK(vkQueueSubmit2(_device->GetGraphicsQueue(), 1, &submitInfo, frame.inFlightFence))

    std::array presentWaitSemaphores = { _renderFinishedSemaphores[imageIndex] };
    if (_swapchain->QueuePresent(imageIndex, presentWaitSemaphores)) {
        RecreateImages();
    }

    _renderData.Reset();


    _currentFrame = (_currentFrame + 1) % VulkanConfig::maxFramesInFlight;
}

void Renderer::CreateGlobalUniform()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings { 1 };
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    _globalDescriptorLayout = _device->AcquireDescriptorSetLayout(bindings);
    _globalBuffer = VulkanBufferFrameRing{ _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::GlobalUBO) };

    std::array<VkDescriptorBufferInfo, VulkanConfig::maxFramesInFlight> descriptorBufferInfos;
    std::array<VkWriteDescriptorSet, VulkanConfig::maxFramesInFlight> descriptorWrites;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _globalDescriptorSets[i] = _descriptorSetCache->Allocate(_globalDescriptorLayout);

        descriptorBufferInfos[i] = _globalBuffer.GetDescriptorInfo(i);

        descriptorWrites[i] = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = _globalDescriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos[i],
        };
    }

    vkUpdateDescriptorSets(_device->Get(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Renderer::TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageSubresourceRange range, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = range;

    vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Renderer::QueueSelectionBuffer()
{
    _queuedSelectionBuffer = true;
}

uint32_t Renderer::GetSelectionValue(const glm::ivec2&)
{
    return 0;
}

void Renderer::AddMaterial(VulkanMaterialInstance* material)
{
    _materials.emplace(material);
}

void Renderer::RemoveMaterial(VulkanMaterialInstance* material)
{
    _materials.erase(material);
}

std::vector<VkPresentModeKHR> Renderer::GetPresentModes()
{
    return _device->GetPhysicalDevice()->GetPresentModes(_window);
}

void Renderer::SetPresentMode(VkPresentModeKHR mode)
{
    recreatePresentMode = mode;
    recreateRequested = true;
}

VkPresentModeKHR Renderer::GetPresentMode() const
{
    return _swapchain->GetPresentMode();
}

std::vector<VkSampleCountFlagBits> Renderer::GetMultisampleCounts()
{
    std::vector<VkSampleCountFlagBits> counts {};
    VkSampleCountFlags flags = _device->GetPhysicalDevice()->GetSupportedFramebufferSampleCounts();

    counts.push_back(VK_SAMPLE_COUNT_1_BIT);
    if (flags & VK_SAMPLE_COUNT_64_BIT) {
        counts.push_back(VK_SAMPLE_COUNT_64_BIT);
    }
    if (flags & VK_SAMPLE_COUNT_32_BIT) {
        counts.push_back(VK_SAMPLE_COUNT_32_BIT);
    }
    if (flags & VK_SAMPLE_COUNT_16_BIT) {
        counts.push_back(VK_SAMPLE_COUNT_16_BIT);
    }
    if (flags & VK_SAMPLE_COUNT_8_BIT) {
        counts.push_back(VK_SAMPLE_COUNT_8_BIT);
    }
    if (flags & VK_SAMPLE_COUNT_4_BIT) {
        counts.push_back(VK_SAMPLE_COUNT_4_BIT);
    }
    if (flags & VK_SAMPLE_COUNT_2_BIT) {
        counts.push_back(VK_SAMPLE_COUNT_2_BIT);
    }

    std::sort(counts.begin(), counts.end());

    return counts;
}

void Renderer::SetMultisampleCount(VkSampleCountFlagBits count)
{
    // Ensure the sample count is supported.
    VkSampleCountFlags flags = _device->GetPhysicalDevice()->GetSupportedFramebufferSampleCounts();
    if (!(flags & count)) {
        Print::Error("Invalid sample count in setting multisample count, not supported.");
        return;
    }

    _changedSampleCount = true;
    _newSampleCount = count;
    recreateRequested = true;
}

VkSampleCountFlagBits Renderer::GetMultisampleCount()
{
    return _sampleCount;
}

std::vector<VkFormat> Renderer::GetColorAttachmentFormats(RenderPassType pass)
{
    std::vector<VkFormat> out;
    switch (pass) {
        case RenderPassType::eGeometry: return { _swapchain->GetFormat(), VK_FORMAT_R32_UINT };
    }

    return out;
}

std::vector<VkPipelineColorBlendAttachmentState> Renderer::GetColorBlendAttachmentStates(RenderPassType)
{

    return {{
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    },
    {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    }};
}

VkFormat Renderer::GetDepthAttachmentFormat(RenderPassType pass)
{
    switch (pass) {
        case RenderPassType::eGeometry: return _depthFormat;
        default: throw std::runtime_error("Invalid enum");
    }
}

VkFormat Renderer::GetStencilAttachmentFormat(RenderPassType pass)
{    switch (pass) {
        case RenderPassType::eGeometry: return VK_FORMAT_UNDEFINED;
        default: throw std::runtime_error("Invalid enum");
    }
}

void Renderer::UpdateMaterialUniforms()
{
    for (auto instance : _materials) {
        instance->UpdateUniforms(_currentFrame);
    }
}

void Renderer::UpdateGlobalUniform()
{
    const auto currentTime = Time::Current();
    const auto extent = _swapchain->GetExtent();

    _uboData.time = currentTime;
    _uboData.dt = _frameCounter.GetDeltaTime();
    _uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    _uboData.mouse = {}; // TODO: mouse position to be added later.

    _globalBuffer.Upload(std::as_bytes(std::span<GlobalUBO, 1>{&_uboData, 1}), _currentFrame);
}



RenderData& Renderer::GetRenderData()
{
    return _renderData;
}

void Renderer::PrepareRenderData(RenderData& rd)
{

    auto& frame = _perFrame[_currentFrame];

    rd.SetCurrentFrame(_currentFrame);
    rd.SetCommandBuffer(frame.commandBuffer);
    rd.SetGlobalDescriptorSet(_globalDescriptorSets[_currentFrame]);
    rd.SetSampleCount(_sampleCount);

    rd.SetDebugMaterialInstance(_pointMaterial, _lineMaterial, _triangleMaterial);
}

void Renderer::SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial)
{
    _pointMaterial = pointMaterial;
    _lineMaterial = lineMaterial;
    _triangleMaterial = triangleMaterial;
}

void Renderer::DrawPoint(const glm::vec3& point, float size, Color color)
{
    _renderData.DrawPoint(point, size, color);
}

void Renderer::DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness, Color color)
{
    _renderData.DrawLine(a, b, thickness, color);
}

void Renderer::DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness, Color color)
{
    _renderData.DrawTriangle(a, b, c, thickness, color);
}


} // namespace bl
