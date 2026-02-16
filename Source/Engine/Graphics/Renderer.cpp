#include "Renderer.h"
#include "Core/FrameCounter.h"
#include "Core/Profiler.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "GraphicsSystem.h"
#include "Passes/SelectionPass.h"
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
    : _frameCounter(frameCounter)
    , _device(window->GetDevice())
    , _window(window)
    , _swapchain(window->GetSwapchain())
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

    assert(_sampleCount > VK_SAMPLE_COUNT_1_BIT);

    try {
        _descriptorSetCache = std::make_unique<VulkanDescriptorSetAllocatorCache>(_device, 1024, VulkanDescriptorRatio::Default());

        auto physicalDevice = _device->GetPhysicalDevice();
        _depthFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, 0);
        _positionFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_R32G32B32A32_SFLOAT }, VK_IMAGE_TILING_OPTIMAL, 0);

        _selectionPass = std::make_unique<SelectionPass>(_device, _swapchain->GetExtent());

        CreateCommandBuffers();
        RecreateImages();
        CreateGlobalUniform();
        CreateDebugBuffer();

    } catch (const std::exception& e) {
        Print::Error("Failed to initialize renderer: {}", e.what());
        DestroyGlobalUniform();
        DestroyImagesAndFramebuffers();
        DestroyCommandBuffers();
        throw e;
    }
}

Renderer::~Renderer()
{
    _device->WaitForDevice();

    DestroyGlobalUniform();
    DestroyImagesAndFramebuffers();
    DestroyCommandBuffers();
}

void Renderer::SetProjection(const glm::mat4& projection)
{
    _uboData.projection = projection;
}

void Renderer::SetView(const glm::mat4& view)
{
    _uboData.view = view;
}

void Renderer::CreateCommandBuffers()
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = VulkanConfig::maxFramesInFlight;

    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, _commandBuffers.data()))
}

void Renderer::DestroyCommandBuffers()
{
    vkFreeCommandBuffers(_device->Get(), _device->GetCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
    _commandBuffers.fill(VK_NULL_HANDLE);
}

void Renderer::DestroyImagesAndFramebuffers()
{
    _colorImage.reset();
    _depthImage.reset();
}

void Renderer::DestroyGlobalUniform()
{
    for (int i = 0; i < VulkanConfig::maxFramesInFlight; i++) {
        _globalBuffer[i].Unmap();
        _descriptorSetCache->Free(_globalLayout, _globalSet[i]);
    }

    _globalSet.fill(VK_NULL_HANDLE);
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

    // Create depth image
    _depthImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, imageExtent, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false, _sampleCount);
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    _depthImageView = std::make_unique<VulkanImageView>(_device, _depthImage.get(), VK_IMAGE_VIEW_TYPE_2D, _depthFormat, mapping, range);

    // Create image views for swapchain images
    auto swapchainImages = _swapchain->GetImages();
    auto swapchainImageViews = _swapchain->GetImageViews();
    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++) {
        _swapchainImages[i] = swapchainImages[i];
        _swapchainImageViews[i] = swapchainImageViews[i];
    }

    _selectionPass->Resize(extent);

    if (_recreateCallback) {
        _recreateCallback();
    }
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

void Renderer::Render(Node* root)
{
}

void Renderer::Render(RenderFunction func, ObjectFunction objectFunc)
{
    // If the window is minimized, we don't draw anything.
    if (_window->GetMinimized())
        return;

    if (recreateRequested) {
        _device->WaitForDevice(); // Wait for previous commands to complete.

        // DestroyRenderPasses();
        // CreateRenderPasses();
        _sampleCount = _newSampleCount;
        _swapchain->Recreate(recreatePresentMode);
        RecreateImages();
        recreateRequested = false;
        _device->WaitForDevice();
    }

    auto currentFrame = _swapchain->GetCurrentFrame();
    _renderData.SetCurrentFrame(currentFrame);

    // Prepare uniform buffers for the next frame.
    UpdateMaterialUniforms(currentFrame);

    // Compute the per frame UBO.
    UpdateGlobalUniform(currentFrame);

    // Swapchain must be valid.
    if (!_swapchain->Get()) {
        return;
    }

    // Acquire the next image in the swapchain and update all render pass
    // images if the swapchain was recreated within the previous frame.
    if (_swapchain->AcquireNext()) {
        RecreateImages();
        return; // skip this frame!
    }

    const auto imageIndex = _swapchain->GetImageIndex();
    _renderData.SetImageIndex(imageIndex);

    auto cmd = _commandBuffers[currentFrame];
    VK_CHECK(vkResetCommandBuffer(cmd, 0))

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo))

    _renderData.SetCommandBuffer(cmd);

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

    // When using a higher sample count, the image must be resolved from the sampled image.
    if (_sampleCount != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachments[0].resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[0].resolveImageView = _swapchainImageViews[imageIndex];
    } else {
        colorAttachments[0].imageView = _swapchainImageViews[imageIndex];
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

    // Transition the swapchain image back into a color attachment.
    TransitionImageLayout(cmd,
        _swapchainImages[imageIndex],
        range,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // If using a multisampled image transition to a color image.
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

    // Transition the depth image.
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
    _renderData.SetGlobalDescriptorSet(_globalSet[currentFrame]);

    VkExtent2D extent = _swapchain->GetExtent();

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { extent.width, extent.height };
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdSetRasterizationSamplesEXT(cmd, _sampleCount);

    _renderData.WriteDrawCommands();

    func(_renderData);

    UpdateDebugBuffers(currentFrame);
    DrawDebugBuffers(_renderData);

    vkCmdEndRendering(cmd);

    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    TransitionImageLayout(cmd,
        _swapchainImages[imageIndex],
        range,
        getPipelineStageFlags(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
        getPipelineStageFlags(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        getAccessFlags(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
        getAccessFlags(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    _selectionPass->Render(_renderData);

    VK_CHECK(vkEndCommandBuffer(cmd))

    _swapchain->QueueSubmit(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    if (_swapchain->QueuePresent()) {
        RecreateImages();
    }

    _renderData.Reset();

    _points.clear();
    _lines.clear();
    _triangles.clear();
}

void Renderer::SetImageRecreateCallback(std::function<void()> onRecreate)
{
    _recreateCallback = onRecreate;
}

uint32_t Renderer::GetSelectionBufferValue(const glm::ivec2& position)
{

    // Transition the image to a readable buffer.
    return 0;
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

void Renderer::SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial)
{
    _pointMaterial = pointMaterial;
    _lineMaterial = lineMaterial;
    _triangleMaterial = triangleMaterial;
}

void Renderer::DrawPoint(const glm::vec3& point, float size, Color color)
{
    _points.emplace_back(point, color.ToVector3(), 0.0f);
}

void Renderer::DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness, Color color)
{
    _lines.emplace_back(a, color.ToVector3(), 0.0f);
    _lines.emplace_back(b, color.ToVector3(), 0.0f);
}

void Renderer::DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness, Color color)
{
    _triangles.emplace_back(a, color.ToVector3(), 0.0f);
    _triangles.emplace_back(b, color.ToVector3(), 0.0f);
    _triangles.emplace_back(c, color.ToVector3(), 0.0f);
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
        case RenderPassType::eGeometry: return { _swapchain->GetFormat() };
        case RenderPassType::eSelection: _selectionPass->GetColorFormats(out); break;
    }

    return out;
}

VkFormat Renderer::GetDepthAttachmentFormat(RenderPassType pass)
{
    switch (pass) {
        case RenderPassType::eGeometry: return _depthFormat;
        case RenderPassType::eSelection: return _selectionPass->GetDepthFormat();
    }
}

VkFormat Renderer::GetStencilAttachmentFormat(RenderPassType pass)
{    switch (pass) {
        case RenderPassType::eGeometry: return VK_FORMAT_UNDEFINED;
        case RenderPassType::eSelection: return _selectionPass->GetStencilFormat();
    }
}

void Renderer::UpdateMaterialUniforms(uint32_t currentFrame)
{
    for (auto instance : _materials) {
        instance->UpdateUniforms(currentFrame);
    }
}

void Renderer::UpdateGlobalUniform(uint32_t currentFrame)
{
    const auto currentTime = Time::Current();
    const auto extent = _swapchain->GetExtent();

    _uboData.time = currentTime;
    _uboData.dt = _frameCounter.GetDeltaTime();
    _uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    _uboData.mouse = {}; // TODO: mouse position to be added later.

    std::memcpy(_globalBufferMap[currentFrame], &_uboData, sizeof(_uboData));

    _globalBuffer[currentFrame].Flush(0, sizeof(_uboData));
}

#define MAX_DEBUG_VERTICES 8196

void Renderer::CreateDebugBuffer()
{
    _lines.reserve(MAX_DEBUG_VERTICES / 3);
    _points.reserve(MAX_DEBUG_VERTICES / 3);
    _triangles.reserve(MAX_DEBUG_VERTICES / 3);
    _debugVertices.reserve(MAX_DEBUG_VERTICES);
    _debugBuffer = VulkanBufferFrameRing(_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, _swapchain->GetImageCount(), MAX_DEBUG_VERTICES * sizeof(VertexDebug), true, false);
}

void Renderer::UpdateDebugBuffers(uint32_t currentFrame)
{
    _debugVertices.clear();

    std::copy_n(_points.begin(), std::min(_debugVertices.capacity(), _points.size()), std::back_inserter(_debugVertices));
    std::copy_n(_lines.begin(), std::min(_debugVertices.capacity(), _lines.size()), std::back_inserter(_debugVertices));
    std::copy_n(_triangles.begin(), std::min(_debugVertices.capacity(), _triangles.size()), std::back_inserter(_debugVertices));

    // Update this current frames buffer.
    _debugBuffer.UploadHostVisible(std::as_bytes(std::span { _debugVertices }), currentFrame);
}

void Renderer::DrawDebugBuffers(RenderData& rd)
{
    // Draw the points list
    if (_points.empty() && _lines.empty() && _triangles.empty())
        return;

    auto cmd = rd.GetCommandBuffer();
    VkDeviceSize vertexOffset = _debugBuffer.GetDynamicOffset(rd.GetCurrentFrame());
    VkBuffer buffer = _debugBuffer.GetBuffer();

    vkCmdSetRasterizationSamplesEXT(cmd, _sampleCount);

    if (_pointMaterial != nullptr && _points.size() > 0) {
        _pointMaterial->Bind(rd);
        vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &vertexOffset);
        vkCmdDraw(cmd, static_cast<uint32_t>(_points.size()), 1, 0, 0);
    }

    // Draw the lines list
    uint32_t firstVertex = static_cast<uint32_t>(_points.size());
    if (_lineMaterial != nullptr && _lines.size() > 0) {
        _lineMaterial->Bind(rd);
        vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &vertexOffset);
        vkCmdSetLineWidth(cmd, 3.0f);
        vkCmdDraw(cmd, static_cast<uint32_t>(_lines.size()), 1, firstVertex, 0);
    }

    // Draw the trangles list
    firstVertex += static_cast<uint32_t>(_lines.size());
    if (_triangleMaterial != nullptr && _triangles.size() > 0) {
        _triangleMaterial->Bind(rd);
        vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &vertexOffset);
        vkCmdDraw(cmd, static_cast<uint32_t>(_triangles.size()), 1, firstVertex, 0);
    }
}

void Renderer::SetSelectionMaterialInstance(VulkanMaterialInstance* instance)
{
    _selectionPass->SetMaterial(instance);
}

} // namespace bl
