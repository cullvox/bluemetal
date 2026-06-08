#include "Renderer.h"
#include "Core/FrameCounter.h"
#include "Core/Profiler.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "Graphics/RenderData.h"
#include "GraphicsSystem.h"
#include "Resources/Mesh.h"
#include "Scene/Node.h"
#include "UniformData.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanMaterial.h"
#include "VulkanWindow.h"
#include <vulkan/vulkan_core.h>

namespace bl {

Renderer::Renderer(VulkanDevice* device, VulkanViewport* mainViewport, FrameCounter& frameCounter)
    : _device(device)
    , _mainViewport(mainViewport)
    , _frameCounter(frameCounter)
    , _renderData(this)
{
    auto physicalDevice = _device->GetPhysicalDevice();

    _colorFormat = physicalDevice->FindSupportedFormat({VK_FORMAT_R8G8B8A8_UNORM}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
    _depthFormat  = physicalDevice->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, 0);

    _descriptorSetCache = std::make_unique<VulkanDescriptorSetAllocatorCache>(_device, 1024, VulkanDescriptorRatio::Default());

    AddViewport(mainViewport);
}

Renderer::~Renderer()
{
    _device->WaitForDevice();

    for (auto& vp : _viewports) {
        DestroyGlobalUniform(vp);
        DestroyImages(vp);
        DestroyPerFrameSyncData(vp);
    }
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

void Renderer::CreatePerFrameSyncData(ViewportData& vp)
{
    auto& sync = vp.syncData;

    // Some viewports use swapchains and require frame synchronization.
    if (!vp.viewport->IsSynchronized()) {
        // This viewport probably renders to a texture, so synchronization is not required.
        sync.requiresSync = false;
        return;
    }

    // Build out the per-frame semaphores infos.
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // Create the semaphores for each frame in flight.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) 
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &sync.imageAvailableSemaphores[i]))
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &sync.inFlightFences[i]))
    }

    // Create a render-finished semaphore for each frame in the synchronization driver.
    auto imageCount = vp.viewport->GetSynchronizedImageCount();
    sync.renderFinishedSemaphores.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++) 
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &sync.renderFinishedSemaphores[i]))
    }
}

void Renderer::CreateCommandBuffers()
{
    // Build out the command buffer info.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = VulkanConfig::maxFramesInFlight;

    // Allocate the per-frame in flight command buffers.
    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, _commandBuffers.data()))


    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // Create the fences for each frame in flight
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) 
    {
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &_inFlightFences[i]))
    }
}

void Renderer::DestroyPerFrameSyncData(ViewportData& vp)
{
    auto& sync = vp.syncData; 

    // Destroy all per-frame in flight semaphores.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        vkDestroySemaphore(_device->Get(), sync.imageAvailableSemaphores[i], nullptr);
    }

    // Destroy all per-image render finished semaphores. 
    for (uint32_t i = 0; i < vp.viewport->GetSynchronizedImageCount(); i++)
    {
        vkDestroySemaphore(_device->Get(), sync.renderFinishedSemaphores[i], nullptr);
    }
}

void Renderer::DestroyImages(ViewportData& vp)
{
    auto& rd = vp.renderData;
 
    rd.colorImage.reset();
    rd.colorImageView.reset();
    rd.colorImageResolved.reset();
    rd.colorImageResolvedView.reset();
    rd.selectionImageSampled.reset();
    rd.selectionImageSampledView.reset();
    rd.selectionImage.reset();
    rd.selectionImageView.reset();
    rd.selectionBuffer.reset();
    rd.depthImage.reset();
    rd.depthImageView.reset();
}

void Renderer::DestroyGlobalUniform(ViewportData& vp)
{
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _descriptorSetCache->Free(_globalDescriptorLayout, vp.guboData.globalDescriptorSets[i]);
    }
}

VulkanDevice* Renderer::GetDevice() const
{
    return _device;
}

void Renderer::RecreateImages(ViewportData& vp)
{
    auto& rd = vp.renderData;

    // Destroy all the images properly before we move the new ones in.
    DestroyImages(vp);

    // Construct all the image buffers for the passes.
    auto extent = vp.viewport->GetExtent();
    VkExtent3D imageExtent =  { extent.width, extent.height, 1 };

    // Create color image
    VkComponentMapping mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    rd.colorImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, false, rd.sampleCount);
    rd.colorImageView = std::make_unique<VulkanImageView>(_device, rd.colorImage.get(), VK_IMAGE_VIEW_TYPE_2D, _colorFormat, mapping, range);

    rd.colorImageResolved = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, false, VK_SAMPLE_COUNT_1_BIT);
    rd.colorImageResolvedView = std::make_unique<VulkanImageView>(_device, rd.colorImageResolved.get(), VK_IMAGE_VIEW_TYPE_2D, _colorFormat, mapping, range);

    // Create selection buffer images
    rd.selectionImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, VK_FORMAT_R32_UINT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    rd.selectionImageView = std::make_unique<VulkanImageView>(_device, rd.selectionImage.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);

    if (rd.sampleCount != VK_SAMPLE_COUNT_1_BIT) 
    {
        rd.selectionImageSampled = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, VK_FORMAT_R32_UINT, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, false, rd.sampleCount);
        rd.selectionImageSampledView = std::make_unique<VulkanImageView>(_device, rd.selectionImageSampled.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);
    }

    rd.selectionBuffer = std::make_unique<VulkanBuffer>(_device, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU, extent.width * extent.height * sizeof(uint32_t), nullptr, true, VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Create depth image
    rd.depthImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false, rd.sampleCount);
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    rd.depthImageView = std::make_unique<VulkanImageView>(_device, rd.depthImage.get(), VK_IMAGE_VIEW_TYPE_2D, _depthFormat, mapping, range);

    // Transition depth image
    _device->ImmediateSubmit([&](VkCommandBuffer cmd){

        std::array<VkImageMemoryBarrier2, 5> barriers = {};
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
        barriers[0].image = rd.depthImage->Get();
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
        barriers[1].image = rd.colorImage->Get();
        barriers[1].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        barriers[2].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[2].pNext = nullptr;
        barriers[2].srcStageMask = 0;
        barriers[2].srcAccessMask = 0;
        barriers[2].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        barriers[2].dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        barriers[2].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[2].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barriers[2].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[2].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[2].image = rd.colorImageResolved->Get();
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
        barriers[3].image = rd.selectionImage->Get();
        barriers[3].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        barriers[4].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[4].pNext = nullptr;
        barriers[4].srcStageMask = 0;
        barriers[4].srcAccessMask = 0;
        barriers[4].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[4].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[4].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[4].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[4].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[4].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[4].image = rd.selectionImageSampled->Get();
        barriers[4].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        VkDependencyInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        info.pNext = nullptr,
        info.dependencyFlags = 0,
        info.imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
        info.pImageMemoryBarriers = barriers.data();

        vkCmdPipelineBarrier2(cmd, &info);
    });
}

Profiler profiler;

void Renderer::Render(Node*)
{
}

bool Renderer::ViewportData::RequiresRecreation()
{
    auto a = viewport->GetExtent();
    auto b = renderData.colorImage->GetExtent();

    return a.width != b.width && a.height != b.height;
}

void Renderer::Render(RenderFunction func, RenderFunction guiPassFunc,  ObjectFunction objectFunc)
{
    // If the window is minimized, we don't draw anything.
    if (!_mainViewport->IsActive())
        return;

    _renderData.SetCurrentFrame(_currentFrame);

    // Wait for the any viewport images coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->Get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX))
    VK_CHECK(vkResetFences(_device->Get(), 1, &_inFlightFences[_currentFrame]))

    // Reset the command buffer for this frame.
    auto cmd = _commandBuffers[_currentFrame];
    VK_CHECK(vkResetCommandBuffer(cmd, 0))

    // Begin this frames command buffer.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo))


    // Reset the submission info.
    _submitWaitInfos.clear();
    _submitSignalInfos.clear();

    // Render all the frame data to the gbuffer.
    _renderData.SetCommandBuffer(cmd);

    // Prepare material uniform buffers for this frame.
    UpdateMaterialUniforms();

    // Compute the object data in the render data.
    objectFunc(_renderData);

    // Write the instance data to the storage buffer.
    _renderData.WriteInstanceBuffer();

    for (auto& viewport : _viewports) {

        auto& sync = viewport.syncData;
        auto& rd = viewport.renderData;
        auto& gubo = viewport.guboData;

        if (viewport.RequiresRecreation()) {
            _device->WaitForDevice(); // Wait for previous commands to complete.    

            // Sample Count Something...

            // Recreate the images.
            RecreateImages(viewport);
            _device->WaitForDevice();
        }

        // Ensure the viewport is ready.
        if (!viewport.viewport->IsActive()) {
            return;
        }

        // Acquire the next image from a synchronized swapchain in the index.
        if (sync.requiresSync) {
            if (viewport.viewport->AcquireNextImage(sync.imageAvailableSemaphores[_currentFrame])) {
                RecreateImages(viewport);
                return; // skip this frame!
            }
        }

        // Compute the per frame per viewport UBO.
        UpdateGlobalUniform(viewport);

        _renderData.SetSampleCount(rd.sampleCount);
        _renderData.SetGlobalDescriptorSet(gubo.globalDescriptorSets[_currentFrame]);

        // Render the scene to the viewport.
        RenderSceneToViewport(_renderData, viewport);

        // Every viewport with a swapchain must be on the frame sync.
        if (viewport.syncData.requiresSync) {
            VkSemaphoreSubmitInfo waitSemaphoreInfo = {};
            waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            waitSemaphoreInfo.pNext = nullptr;
            waitSemaphoreInfo.semaphore = sync.imageAvailableSemaphores[_currentFrame];
            waitSemaphoreInfo.value = 0;
            waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            waitSemaphoreInfo.deviceIndex = 0;

            _submitWaitInfos.push_back(waitSemaphoreInfo);

            VkSemaphoreSubmitInfo signalSemaphoreInfo = {};
            signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            signalSemaphoreInfo.pNext = nullptr;
            signalSemaphoreInfo.semaphore = sync.renderFinishedSemaphores[viewport.viewport->GetCurrentImageIndex()];
            signalSemaphoreInfo.value = 0;
            signalSemaphoreInfo.stageMask = 0;
            signalSemaphoreInfo.deviceIndex = 0;

            _submitSignalInfos.push_back(signalSemaphoreInfo);
        }

        if ()
    }

    // Setup the swapchain image for the GUI pass.
    VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    _mainViewport->GetImage()->Transition(
        cmd, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
        0, 
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 
        range);

    ViewportData* mainViewportData = nullptr;
    for (int i = 0; i < _viewports.size(); i++) {
        if (_viewports[i].viewport == _mainViewport) {
            mainViewportData = &_viewports[i];
        }
    }

    assert(mainViewportData != nullptr);

    RenderUIToViewport(guiPassFunc, _renderData, *mainViewportData);

    // Barrier transition the swapchain image to a presentable layout.
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    // Transition the swapchain image to present src for presentation.
    _mainViewport->GetImage()->Transition(
        cmd, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, 
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 
        0, 
        range);

    // Transition the color attachment (now a sampled image) back to a color attachment for the next frame.
    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT) {
        barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        barriers[1].pNext = nullptr;
        barriers[1].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        barriers[1].srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        barriers[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barriers[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barriers[1].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barriers[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].image = _colorImage->Get();
        barriers[1].subresourceRange = range;

        info.imageMemoryBarrierCount = 2;
    }

    vkCmdPipelineBarrier2(cmd, &info);

    VK_CHECK(vkEndCommandBuffer(cmd))

    // Submit the command buffer to the graphics queue.
    VkCommandBufferSubmitInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferInfo.pNext = nullptr;
    commandBufferInfo.commandBuffer = cmd;
    commandBufferInfo.deviceMask = 0;

    VkSubmitInfo2 submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.pNext = nullptr;
    submitInfo.flags = 0;
    submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(_submitWaitInfos.size());
    submitInfo.pWaitSemaphoreInfos = _submitWaitInfos.data();
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferInfo;
    submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(_submitSignalInfos.size());
    submitInfo.pSignalSemaphoreInfos = _submitSignalInfos.data();

    VK_CHECK(vkQueueSubmit2(_device->GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]))

    for (auto& vp : _viewports) {
        if (vp.syncData.requiresSync) {
            if (vp.viewport->Present(vp.syncData.renderFinishedSemaphores[vp.viewport->GetCurrentImageIndex()])) {
                RecreateImages(vp);
            }
        }
    }

    _renderData.Reset();

    _currentFrame = (_currentFrame + 1) % VulkanConfig::maxFramesInFlight;
}

void Renderer::Render(VulkanViewport& viewport, RenderData& data)
{

}

void Renderer::CreateGlobalUniform(ViewportData& vp)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings { 1 };
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    _globalDescriptorLayout = _device->AcquireDescriptorSetLayout(bindings);
    vp.guboData.globalBuffer = VulkanBufferFrameRing{ _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::GlobalUBO) };

    std::array<VkDescriptorBufferInfo, VulkanConfig::maxFramesInFlight> descriptorBufferInfos;
    std::array<VkWriteDescriptorSet, VulkanConfig::maxFramesInFlight> descriptorWrites;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        vp.guboData.globalDescriptorSets[i] = _descriptorSetCache->Allocate(_globalDescriptorLayout);

        descriptorBufferInfos[i] = vp.guboData.globalBuffer.GetDescriptorInfo(i);

        descriptorWrites[i] = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = vp.guboData.globalDescriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos[i],
        };
    }

    vkUpdateDescriptorSets(_device->Get(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

// void Renderer::TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageSubresourceRange range, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
// {
//     VkImageMemoryBarrier barrier = {};
//     barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//     barrier.pNext = nullptr;
//     barrier.srcAccessMask = srcAccessMask;
//     barrier.dstAccessMask = dstAccessMask;
//     barrier.oldLayout = oldLayout;
//     barrier.newLayout = newLayout;
//     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//     barrier.image = image;
//     barrier.subresourceRange = range;

//     vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
// }

// void Renderer::QueueSelectionBuffer(ViewportData& vp)
// {
//     vp.renderData.queuedSelectionBuffer = true;
// }

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
        case RenderPassType::eGeometry: return { VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R32_UINT };
    }

    return out;
}

std::vector<VkPipelineColorBlendAttachmentState> Renderer::GetColorBlendAttachmentStates(RenderPassType)
{

    return {{ // Color Buffer
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    },
    { // Selection Buffer
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

VulkanDescriptorSetAllocatorCache* Renderer::GetDescriptorSetAllocatorCache()
{
    return _descriptorSetCache.get();
}

void Renderer::UpdateMaterialUniforms()
{
    for (auto instance : _materials) {
        instance->UpdateUniforms(_currentFrame);
    }
}

void Renderer::AddViewport(VulkanViewport* viewport)
{
    ViewportData newViewportData;
    newViewportData.viewport = viewport;

    CreateGlobalUniform(newViewportData);
    CreatePerFrameSyncData(newViewportData);
    RecreateImages(newViewportData);
}

void Renderer::UpdateGlobalUniform(ViewportData& vp)
{
    const auto currentTime = Time::Current();
    const auto extent = vp.viewport->GetExtent();

    _uboData.time = currentTime;
    _uboData.dt = _frameCounter.GetDeltaTime();
    _uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    _uboData.mouse = {}; // TODO: mouse position to be added later.

    auto& ubo = vp.guboData.globalBuffer;
    ubo.Upload(std::as_bytes(std::span<GlobalUBO, 1>{&_uboData, 1}), _currentFrame);
}

VulkanImageView* Renderer::GetColorImageView()
{
    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        return _colorImageResolvedView.get();
    }
    return _colorImageView.get();
}

RenderData& Renderer::GetRenderData()
{
    return _renderData;
}

float Renderer::GetCurrentFrameTime()
{
    return _frameCounter.GetBeginFrameTime();
}

float Renderer::GetCurrentFrameDeltaTime()
{
    return _frameCounter.GetDeltaTime();
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


void Renderer::RenderSceneToViewport(RenderData& rd, ViewportData& vp)
{
    auto cmd = rd.GetCommandBuffer();

    auto& renderData = vp.renderData;

    // Update the viewports global uniform buffer object.
    UpdateGlobalUniform(vp);

    // Setup the render pass for dynamic rendering.
    std::array clearColors = {
        VkClearValue { .color = { { 0.96f, 0.97f, 0.96f, 1.0f } } }, // Clear Color
        VkClearValue { .depthStencil = { 1.0f, 0 } }, // Clear Depth
        VkClearValue { .color = { -1, -1, -1, -1 } }
    };

    auto extent = vp.viewport->GetExtent();

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = extent;

    std::array<VkRenderingAttachmentInfo, 2> colorAttachments = {};
    colorAttachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[0].pNext = nullptr;
    colorAttachments[0].imageView = vp.viewport->GetImageView()->Get();
    colorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[0].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[0].clearValue = clearColors[0];

    colorAttachments[1].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[1].pNext = nullptr;
    colorAttachments[1].imageView = renderData.selectionImageView->Get();
    colorAttachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[1].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[1].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[1].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[1].clearValue = VkClearValue { .color = { -1, -1, -1, -1 } };

    // When using a higher sample count, the image must be resolved from the sampled image.
    if (renderData.sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachments[0].imageView = renderData.colorImageView->Get();
        colorAttachments[0].resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[0].resolveImageView = vp.viewport->GetImageView()->Get();

        colorAttachments[1].imageView = renderData.selectionImageSampledView->Get();
        colorAttachments[1].resolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
        colorAttachments[1].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[1].resolveImageView = renderData.selectionImageView->Get();
    }

    VkRenderingAttachmentInfo depthAttachment = {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.pNext = nullptr;
    depthAttachment.imageView = renderData.depthImageView->Get();
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

    vkCmdBeginRendering(cmd, &renderingInfo);

    // Set the viewport and scissor sizing for this viewport render.
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

    // Write the scenes draw commands to the command buffer.
    _renderData.SetGlobalDescriptorSet(vp.guboData.globalDescriptorSets[_currentFrame]);
    _renderData.SetSampleCount(renderData.sampleCount);
    _renderData.WriteDrawCommands();

    // End the scene geometry pass.
    vkCmdEndRendering(cmd);
}

void Renderer::RenderUIToViewport(RenderFunction guiFunc, RenderData& rd, ViewportData& vp)
{
    auto cmd = rd.GetCommandBuffer();

    // Setup the render pass for dynamic rendering.
    std::array clearColors = {
        VkClearValue { .color = { { 0.96f, 0.97f, 0.96f, 1.0f } } }, // Clear Color
        VkClearValue { .depthStencil = { 1.0f, 0 } }, // Clear Depth
        VkClearValue { .color = { -1, -1, -1, -1 } }
    };

    auto extent = vp.viewport->GetExtent();

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = extent;

    VkRenderingAttachmentInfo colorAttachment = {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.pNext = nullptr;
    colorAttachment.imageView = vp.viewport->GetImageView()->Get();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachment.resolveImageView = VK_NULL_HANDLE;
    colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = clearColors[0];

    // Setup color attachments for the GUI pass.
    VkRenderingInfo renderingInfo = {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.pNext = nullptr;
    renderingInfo.flags = 0;
    renderingInfo.renderArea = renderArea;
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = nullptr;
    renderingInfo.pStencilAttachment = nullptr;

    // Begin the GUI render pass.
    vkCmdBeginRendering(cmd, &renderingInfo);

    // Set the viewport and scissor.
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewportWithCount(cmd, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { extent.width, extent.height };
    vkCmdSetScissorWithCount(cmd, 1, &scissor);

    _renderData.SetSampleCount(VK_SAMPLE_COUNT_1_BIT);

    guiFunc(_renderData);

    vkCmdEndRendering(cmd);
}

} // namespace bl
