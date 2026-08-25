#include "Renderer.h"
#include "Core/FrameCounter.h"
#include "Core/Profiler.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "Graphics/RenderData.h"
#include "GraphicsSystem.h"
#include "Precompiled.h"
#include "Scene/Node.h"
#include "UniformData.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanMaterial.h"
#include "Viewport.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"
#include <SDL3/SDL_video.h>
#include <vulkan/vulkan_core.h>

namespace bl {

Renderer::Renderer(VulkanDevice* device)
    : _renderData(this)
    , _device(device)
{

    // Determine the renderer image formats.
    auto physicalDevice = _device->GetPhysicalDevice();

    // Create a dummy window to get the acceptable surface formats.
    SDL_Window* tempWindow = SDL_CreateWindow("", 0, 0, SDL_WINDOW_VULKAN);
    if (!tempWindow)
    {
        throw std::runtime_error("Could not create a temporary window to get surface formats.");
    }

    // Create a vulkan surface used to query for surface formats.
    VkSurfaceKHR tempSurface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(tempWindow, _device->GetInstance()->Get(), nullptr, &tempSurface))
    {
        SDL_DestroyWindow(tempWindow);
        throw std::runtime_error("Could not create a temporary vulkan surface to get surface formats.");
    }

    // Query surface formats.
    _surfaceFormats = _device->GetPhysicalDevice()->GetSurfaceFormats(tempSurface);

    // Destroy temporary resources for surface format query.
    vkDestroySurfaceKHR(_device->GetInstance()->Get(), tempSurface, nullptr);
    SDL_DestroyWindow(tempWindow);


    _colorFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    _colorFormatHDR = physicalDevice->FindSupportedFormat({VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    _depthFormat  = physicalDevice->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, 0);
    _selectionFormat = _device->GetPhysicalDevice()->FindSupportedFormat({VK_FORMAT_R32_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    // Create the descriptor set allocator cache.
    _descriptorSetCache = std::make_unique<VulkanDescriptorSetAllocatorCache>(_device, 1024, VulkanDescriptorRatio::Default());

    // Create in-flight fences.
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // Create semaphores for each frame in flight.
    _inFlightFences.resize(VulkanConfig::maxFramesInFlight);
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) 
    {
        VK_CHECK(vkCreateFence(_device->Get(), &fenceInfo, nullptr, &_inFlightFences[i]))
    }

    // Build out the command buffer info.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _device->GetCommandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = VulkanConfig::maxFramesInFlight;

    // Allocate the per-frame in flight command buffers.
    VK_CHECK(vkAllocateCommandBuffers(_device->Get(), &allocateInfo, _commandBuffers.data()))

}

Renderer::~Renderer()
{
    _device->WaitForDevice();

    // Destroy all per-frame in flight semaphores.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        vkDestroyFence(_device->Get(), _inFlightFences[i], nullptr);
    }
}

VulkanDevice* Renderer::GetDevice() const
{
    return _device;
}

Profiler profiler;

void Renderer::RenderFrame()
{
    _renderData.SetCurrentFrame(_currentFrame);

    // Wait for the any viewport images coming in the chain to finish.
    VK_CHECK(vkWaitForFences(_device->Get(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX))
    VK_CHECK(vkResetFences(_device->Get(), 1, &_inFlightFences[_currentFrame]))

    // Reset the command buffer for this frame.
    auto cmd = _commandBuffers[_currentFrame];
    VK_CHECK(vkResetCommandBuffer(cmd, 0))

    // Apply the deleter queue for this frame.
    _deletionQueues[_currentFrame].clear();

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
    _objectFunc(_renderData);

    // Write the instance data to the storage buffer.
    _renderData.WriteInstanceBuffer();

    for (auto viewport : _viewports)
    {
        viewport->PrepareForFrame(_renderData);
    }

    for (auto& viewport : _viewports)
    {
        if (!viewport->Ready())
        {
            continue;
        }

        viewport->TransitionPreRender(_renderData);

        // Render the scene to the viewport.
        RenderSceneToViewport(_renderData, *viewport);

        viewport->TransitionPostRender(_renderData);
    }

    // Transition viewports for possible present.
    for (auto& viewport : _viewports)
    {
        viewport->TransitionPrePresent(_renderData);
    }

    VK_CHECK(vkEndCommandBuffer(cmd))

    // Submit the command buffer to the graphics queue.
    VkCommandBufferSubmitInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    commandBufferInfo.pNext = nullptr;
    commandBufferInfo.commandBuffer = cmd;
    commandBufferInfo.deviceMask = 0;

    auto& waitSemaphores = _renderData.GetRenderWaitSemaphores();
    auto& signalSemaphores = _renderData.GetRenderSignalSemaphores();

    VkSubmitInfo2 submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.pNext = nullptr;
    submitInfo.flags = 0;
    submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphoreInfos = waitSemaphores.data();
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferInfo;
    submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphoreInfos = signalSemaphores.data();

    VK_CHECK(vkQueueSubmit2(_device->GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]))

    // Present any viewports that need to be presented.
    for (auto& vp : _viewports) 
    {
        vp->QueuePresent(_renderData);
    }

    _renderData.Reset();

    _currentFrame = (_currentFrame + 1) % VulkanConfig::maxFramesInFlight;
}

void Renderer::AddMaterial(VulkanMaterialInstance* material)
{
    _materials.emplace(material);
}

void Renderer::RemoveMaterial(VulkanMaterialInstance* material)
{
    _materials.erase(material);
}

void Renderer::AddToDeletionQueue(std::unique_ptr<VulkanResource> resource)
{
    _deletionQueues[_currentFrame].push_back(std::move(resource));
}

std::vector<VkFormat> Renderer::GetColorAttachmentFormats(RenderPassType pass)
{
    std::vector<VkFormat> out;
    switch (pass) {
        case RenderPassType::eGeometry: return { _colorFormat, _selectionFormat };
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

VkFormat Renderer::GetViewportColorFormat()
{
    return _colorFormat;
}

VkFormat Renderer::GetViewportDepthFormat()
{
    return _depthFormat;
}

VkFormat Renderer::GetViewportSelectionFormat()
{
    return _selectionFormat;
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

void Renderer::AddViewport(Viewport* viewport)
{
    _viewports.push_back(viewport);
    std::sort(_viewports.begin(), _viewports.end(), [](Viewport* a, Viewport* b){
        return a->GetRenderingPriority() > b->GetRenderingPriority();
    });
}

RenderData& Renderer::GetRenderData()
{
    return _renderData;
}

float Renderer::GetCurrentFrameTime()
{
    return GetEngine()->GetFrameCounter()->GetBeginFrameTime();
}

float Renderer::GetCurrentFrameDeltaTime()
{
    return GetEngine()->GetFrameCounter()->GetDeltaTime();
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

void Renderer::RenderSceneToViewport(RenderData& rd, Viewport& vp)
{
    auto cmd = rd.GetCommandBuffer();

    vp.UpdateUniform(_renderData);
    vp.Bind(rd);

    // Setup the render pass for dynamic rendering.
    std::array clearColors = {
        VkClearValue { .color = { { 0.96f, 0.97f, 0.96f, 1.0f } } }, // Clear Color
        VkClearValue { .depthStencil = { 1.0f, 0 } }, // Clear Depth
        VkClearValue { .color = { -1, -1, -1, -1 } }
    };

    auto extent = vp.GetExtent();

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = extent;

    std::vector<VkRenderingAttachmentInfo> colorAttachments;
    vp.GetColorRenderingAttachments(colorAttachments);
    
    VkRenderingAttachmentInfo depthAttachment;
    vp.GetDepthRenderingAttachment(depthAttachment);

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

    // Update the viewports global uniform buffer object.
    vp.UpdateUniform(rd);

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
    _renderData.WriteDrawCommands();

    // End the scene geometry pass.
    vkCmdEndRendering(cmd);

    // Render geometry if it's required.
    if (HasFlag(vp.GetRenderFlags(), ViewportRenderFlags::eImGui))
    {
        vp.FillColorRenderingAttachmentsForUI(colorAttachments);

        // Setup color attachments for the GUI pass.
        renderingInfo.renderArea = renderArea;
        renderingInfo.layerCount = 1;
        renderingInfo.viewMask = 0;
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
        renderingInfo.pColorAttachments = colorAttachments.data();
        renderingInfo.pDepthAttachment = nullptr;
        renderingInfo.pStencilAttachment = nullptr;

        // Begin the GUI render pass.
        vkCmdBeginRendering(cmd, &renderingInfo);

        // Set the viewport and scissor.
        vp.Bind(rd);

        _guiFunc(_renderData);

        vkCmdEndRendering(cmd);
    }
}

void Renderer::SetGUIFunction(RenderFunction func)
{
    _guiFunc = func;
}

void Renderer::SetObjectFunction(ObjectFunction func)
{
    _objectFunc = func;
}

} // namespace bl
