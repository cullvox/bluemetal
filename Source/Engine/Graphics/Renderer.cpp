#include "Renderer.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "GraphicsSystem.h"
#include "UniformData.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanMaterial.h"
#include "VulkanWindow.h"
#include "VulkanImage.h"
#include "Resources/Mesh.h"
#include "VulkanImageView.h"

namespace bl {

Renderer::Renderer(VulkanWindow* window)
    : _device(window->GetDevice())
    , _window(window)
    , _swapchain(window->GetSwapchain())
    , _imageIndex(0)
    , _currentFrame(0)
{
    _commandBuffers.resize(VulkanConfig::maxFramesInFlight);
    _imageAvailableSemaphores.resize(VulkanConfig::maxFramesInFlight);
    _renderFinishedSemaphores.resize(_swapchain->GetImageCount());
    _inFlightFences.resize(VulkanConfig::maxFramesInFlight);

    for (VkSampleCountFlagBits flag : GetMultisampleCounts())
    {
        if (flag & VK_SAMPLE_COUNT_8_BIT) _sampleCount = VK_SAMPLE_COUNT_8_BIT;
        if (flag & VK_SAMPLE_COUNT_4_BIT) _sampleCount = VK_SAMPLE_COUNT_4_BIT;
        if (flag & VK_SAMPLE_COUNT_2_BIT) _sampleCount = VK_SAMPLE_COUNT_2_BIT;
    }

    assert(_sampleCount > VK_SAMPLE_COUNT_1_BIT);

    try {
        _descriptorSetCache = std::make_unique<VulkanDescriptorSetAllocatorCache>(_device, 1024, VulkanDescriptorRatio::Default());

        auto physicalDevice = _device->GetPhysicalDevice();
        _depthFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, 0);
        _positionFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_R32G32B32A32_SFLOAT }, VK_IMAGE_TILING_OPTIMAL, 0);

        CreateSyncObjects();
        RecreateImages();
        CreateGlobalUniform();
    } catch (const std::exception& e) {
        Print::Error("Failed to initialize renderer: {}", e.what());
        DestroyGlobalUniform();
        DestroyImagesAndFramebuffers();
        DestroySyncObjects();
        throw e;
    }
}

Renderer::~Renderer()
{
    _device->WaitForDevice();

    DestroyGlobalUniform();
    DestroyImagesAndFramebuffers();
    DestroySyncObjects();
}

void Renderer::SetProjection(const glm::mat4& projection)
{
    _uboData.projection = projection;
}

void Renderer::SetView(const glm::mat4& view)
{
    _uboData.view = view;
}

void Renderer::CreateSyncObjects()
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = VulkanConfig::maxFramesInFlight;

    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, _commandBuffers.data()))

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]))
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &_inFlightFences[i]))
    }

    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++) {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
    }
}

void Renderer::DestroySyncObjects()
{
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) {
        vkDestroySemaphore(_device->Get(), _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(_device->Get(), _inFlightFences[i], nullptr);
    }

    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++) {
        vkDestroySemaphore(_device->Get(), _renderFinishedSemaphores[i], nullptr);
    }

    vkFreeCommandBuffers(_device->Get(), _device->GetCommandPool(), (uint32_t)_commandBuffers.size(), _commandBuffers.data());
}

void Renderer::DestroyImagesAndFramebuffers()
{
    _colorImage.reset();
    _depthImage.reset();
}

void Renderer::DestroyGlobalUniform()
{
    for (int i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _globalBuffer[i].Unmap();
        _descriptorSetCache->Free(_globalLayout, _globalSet[i]);
    }

    _globalSet.fill(VK_NULL_HANDLE);
}

void Renderer::RecreateImages()
{
    _imageCount = _swapchain->GetImageCount();
    auto extent = _swapchain->GetExtent();

    DestroyImagesAndFramebuffers();

    // Construct all the image buffers for the passes.
    auto imageExtent = VkExtent3D { extent.width, extent.height, 1 };

    VkComponentMapping mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    _colorImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _swapchain->GetFormat(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, 1, _sampleCount);
    _colorImageView = std::make_unique<VulkanImageView>(_device, _colorImage.get(), VK_IMAGE_VIEW_TYPE_2D, _swapchain->GetFormat(), mapping, range);
    _depthImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, _sampleCount);
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    _depthImageView = std::make_unique<VulkanImageView>(_device, _depthImage.get(), VK_IMAGE_VIEW_TYPE_2D, _depthFormat, mapping, range);

    _swapchainImages = _swapchain->GetImages();
    _swapchainImageViews = _swapchain->GetImageViews();

    if (_recreateCallback) {
        _recreateCallback();
    }
}


VkPipelineStageFlags getPipelineStageFlags(VkImageLayout layout)
{
	switch (layout)
	{
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
	switch (layout)
	{
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

void Renderer::Render(RenderFunction func)
{
    // If the window is minimized, we don't draw anything.
    if (_window->GetMinimized())
        return;

    // Clear previous frame index data.
    for (auto [_, data] : _instanceDraws) {
        auto& vec = std::get<1>(data);
        vec.clear();
    }

    if (recreateRequested) {
        _device->WaitForDevice(); // Wait for previous commands to complete.

        //DestroyRenderPasses();
        //CreateRenderPasses();
        _swapchain->Recreate(recreatePresentMode);
        RecreateImages();
        recreateRequested = false;
        _device->WaitForDevice();
    }

    // Compute the per frame UBO.
    const auto currentTime = Time::Current();
    const auto extent = _swapchain->GetExtent();

    _uboData.time = currentTime;
    _uboData.dt = currentTime - _prevTime;
    _uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    _uboData.mouse = {}; // TODO: mouse position to be added later.

    _prevTime = currentTime;

    std::memcpy(_globalBufferMap[_currentFrame], &_uboData, sizeof(_uboData));

    _globalBuffer[_currentFrame].Flush(0, sizeof(_uboData));

    // Swapchain must be valid.
    if (!_swapchain->Get()) {
        return;
    }

    // Wait for the current image up coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->Get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX))

    // Acquire the next image in the swapchain and update all render pass
    // images if the swapchain was recreated within the previous frame.
    if (_swapchain->AcquireNext(_imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE)) {
        RecreateImages();
        return; // skip this frame!
    }

    _imageIndex = _swapchain->GetImageIndex();



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

    // Update all material buffers.
    for (auto instance : _materials) {
        instance->UpdateUniforms(cmd);
    }

    std::array clearColors = {
        VkClearValue { .color = { { 0.96f, 0.97f, 0.96f, 1.0f } } }, // Swapchain Image Clear Color
        VkClearValue { .depthStencil = { 1.0f, 0 } }
    };

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = _swapchain->GetExtent();

    std::array<VkRenderingAttachmentInfo, 1> colorAttachments = {};
    colorAttachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[0].pNext = nullptr;
    colorAttachments[0].imageView = _colorImageView->Get();
    colorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[0].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[0].clearValue = clearColors[0];

    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachments[0].resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[0].resolveImageView = _swapchainImageViews[_imageIndex];
    } else {
        colorAttachments[0].imageView = _swapchainImageViews[_imageIndex];
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

    TransitionImageLayout(cmd,
        _swapchainImages[_imageIndex],
        range,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        TransitionImageLayout(cmd,
            _colorImage->Get(),
            range,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    TransitionImageLayout(cmd,
        _depthImage->Get(),
        range,
        getPipelineStageFlags(VK_IMAGE_LAYOUT_UNDEFINED),
        getPipelineStageFlags(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL),
        getAccessFlags(VK_IMAGE_LAYOUT_UNDEFINED),
        getAccessFlags(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(cmd, &renderingInfo);

    // Render all the frame data to the gbuffer.
    VulkanRenderData rd = {
        this,
        cmd,
        _currentFrame,
        _imageIndex,
        _globalSet[_currentFrame]
    };

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(rd.cmd, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { extent.width, extent.height };
    vkCmdSetScissor(rd.cmd, 0, 1, &scissor);

    vkCmdSetRasterizationSamplesEXT(cmd, _sampleCount);

    func(rd);

    // Render instances
    for (auto [mesh, vec] : _instanceDraws) {
        mesh->Bind(cmd);
        
    }

    vkCmdEndRendering(cmd);

    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    TransitionImageLayout(cmd,
        _swapchainImages[_imageIndex],
        range,
        getPipelineStageFlags(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
        getPipelineStageFlags(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        getAccessFlags(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
        getAccessFlags(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VK_CHECK(vkEndCommandBuffer(cmd))

    // Submit the command buffer to the graphics queue.
    std::array waitSemaphores = { _imageAvailableSemaphores[_currentFrame] };
    std::array commandBuffers = { _commandBuffers[_currentFrame] };
    std::array signalSemaphores = { _renderFinishedSemaphores[_imageIndex] };
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

    if (_swapchain->QueuePresent(_renderFinishedSemaphores[_imageIndex])) {
        RecreateImages();
    }

    _currentFrame = (_currentFrame + 1) % VulkanConfig::maxFramesInFlight;
}

void Renderer::SetImageRecreateCallback(std::function<void()> onRecreate)
{
    _recreateCallback = onRecreate;
}

void Renderer::CreateGlobalUniform()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings { 1 };
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    _globalLayout = _device->AcquireDescriptorSetLayout(bindings);

    for (int i = 0; i < VulkanConfig::maxFramesInFlight; i++) {
        _globalBuffer[i] = bl::VulkanBuffer { _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::GlobalUBO), nullptr };
        _globalBuffer[i].Map(&_globalBufferMap[i]);

        _globalSet[i] = _descriptorSetCache->Allocate(_globalLayout);

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = _globalBuffer[i].Get();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;
        write.dstSet = _globalSet[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(_device->Get(), 1, &write, 0, nullptr);
    }

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

void Renderer::AddInstance(Mesh* mesh, const InstanceData& data)
{
    int& index = std::get<int>(_instanceDraws[mesh]);
    std::vector<InstanceData>& vec = std::get<std::vector<InstanceData>>(_instanceDraws[mesh]);

    if (index >= 1000) {
        Print::Warn("Cannot index more than 1000 objects!");
    }

    if (vec.size() != 1000) {
        vec.resize(1000);
    }

    vec[index] = data;
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
    std::vector<VkSampleCountFlagBits> counts{};
    VkSampleCountFlags flags = _device->GetPhysicalDevice()->GetSupportedFramebufferSampleCounts();

    counts.push_back(VK_SAMPLE_COUNT_1_BIT);
    if (flags & VK_SAMPLE_COUNT_64_BIT) { counts.push_back(VK_SAMPLE_COUNT_64_BIT); }
    if (flags & VK_SAMPLE_COUNT_32_BIT) { counts.push_back(VK_SAMPLE_COUNT_32_BIT); }
    if (flags & VK_SAMPLE_COUNT_16_BIT) { counts.push_back(VK_SAMPLE_COUNT_16_BIT); }
    if (flags & VK_SAMPLE_COUNT_8_BIT) { counts.push_back(VK_SAMPLE_COUNT_8_BIT); }
    if (flags & VK_SAMPLE_COUNT_4_BIT) { counts.push_back(VK_SAMPLE_COUNT_4_BIT); }
    if (flags & VK_SAMPLE_COUNT_2_BIT) { counts.push_back(VK_SAMPLE_COUNT_2_BIT); }

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

    _sampleCount = count;
    recreateRequested = true;
}

VkSampleCountFlagBits Renderer::GetMultisampleCount()
{
    return _sampleCount;
}

std::vector<VkFormat> Renderer::GetColorAttachmentFormats()
{
    return { _swapchain->GetFormat() };
}

VkFormat Renderer::GetDepthAttachmentFormat()
{
    return _depthFormat;
}

VkFormat Renderer::GetStencilAttachmentFormat()
{
    return VK_FORMAT_UNDEFINED;
}


} // namespace bl
