#include "Renderer.h"
#include "Core/Print.h"
#include "Graphics/DescriptorSetCache.h"
#include "vulkan/vulkan_core.h"

namespace bl {


Renderer::Renderer(Device* device, Window* window)
    : _device(device)
    , _window(window)
    , _swapchain(window->GetSwapchain())
    , _currentFrame(0)
    , _descriptorSetCache(_device, 1024, DescriptorRatio::Default())
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

void Renderer::RecreatePasses()
{
    VkExtent2D extent = _swapchain->GetExtent();
    DestroyImagesAndFramebuffers();

    _lightImages.reserve(imageCount);
    _depthImages.reserve(imageCount);
    _albedoImages.reserve(imageCount);
    _normalImages.reserve(imageCount);

    auto imageExtent = VkExtent3D{extent.width, extent.height, 1};

    for (uint32_t i = 0; i < imageCount; i++) {
        _lightImages.push_back(std::make_unique<Image>(_device, VK_IMAGE_TYPE_2D, imageExtent, _lightFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
        _depthImages.push_back(std::make_unique<Image>(_device, VK_IMAGE_TYPE_2D, imageExtent, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT));
        _albedoImages.push_back(std::make_unique<Image>(_device, VK_IMAGE_TYPE_2D, imageExtent, _albedoFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
        _normalImages.push_back(std::make_unique<Image>(_device, VK_IMAGE_TYPE_2D, imageExtent, _normalFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
    }

    _framebuffers.reserve(imageCount);

    for (uint32_t i = 0; i < imageCount; i++) {
        std::array attachments = { 
            _lightImages[i]->GetView(), 
            _depthImages[i]->GetView(), 
            _albedoImages[i]->GetView(), 
            _normalImages[i]->GetView(),
        };

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = _pass;
        createInfo.attachmentCount = attachments.size();
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
        RecreatePasses();
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

    if (_swapchain->QueuePresent(_renderFinishedSemaphores[_currentFrame])) {
        RecreatePasses();
    }

    _currentFrame = (_currentFrame + 1) % GraphicsConfig::numFramesInFlight;
}

void Renderer::CreateRenderPasses() {

    // Find the formats for each image in the pass.
    auto physicalDevice = _device->GetPhysicalDevice();
    _lightFormat = physicalDevice->FindSupportedFormat({VK_FORMAT_R32G32B32_SFLOAT}, VK_IMAGE_TILING_OPTIMAL, 0);
    _depthFormat = physicalDevice->FindSupportedFormat({VK_FORMAT_D32_SFLOAT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    _albedoFormat = physicalDevice->FindSupportedFormat({VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8_SRGB}, VK_IMAGE_TILING_OPTIMAL, 0);
    _normalFormat = physicalDevice->FindSupportedFormat({VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_FORMAT_R16G16B16_SFLOAT, VK_FORMAT_R16G16B16A16_SFLOAT}, VK_IMAGE_TILING_OPTIMAL, 0);

    // Build the renderpasses attachment data.
    std::array<VkAttachmentDescription, 4> attachments = {};
    
    // Light Attachment
    attachments[0].flags = 0;
    attachments[0].format = _lightFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Depth Attachment
    attachments[1].flags = 0;
    attachments[1].format = _depthFormat; // hopefully VK_FORMAT_D32_SFLOAT
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // stencil may be used later
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    // Albedo Attachment
    attachments[2].flags = 0;
    attachments[2].format = _albedoFormat;
    attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[2].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Normal Attachment
    attachments[3].flags = 0;
    attachments[3].format = _normalFormat;
    attachments[3].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[3].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[3].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[3].finalLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

    // Swapchain Present Attachment
    attachments[4].flags = 0;
    attachments[4].format = _swapchain->GetFormat();
    attachments[4].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[4].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[4].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[4].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[4].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[4].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference lightAttachmentReference = {};
    lightAttachmentReference.attachment = 0;
    lightAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    VkAttachmentReference albedoAttachmentReference = {};
    albedoAttachmentReference.attachment = 2;
    albedoAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference normalAttachmentReference = {};
    normalAttachmentReference.attachment = 3;
    normalAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference compositeAttachmentReference = {};
    compositeAttachmentReference.attachment = 4;
    compositeAttachmentReference.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    std::array<VkSubpassDescription, 3> subpasses;

    // Geometry Subpass
    std::array geometryColorAttachments = { albedoAttachmentReference, normalAttachmentReference };

    subpasses[0].flags = {};
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = (uint32_t)geometryColorAttachments.size();
    subpasses[0].pColorAttachments = geometryColorAttachments.data();
    subpasses[0].pResolveAttachments = nullptr;
    subpasses[0].pDepthStencilAttachment = &depthAttachmentReference;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;

    // Lighting Subpass
    std::array lightingInputAttachments = { depthAttachmentReference };
    std::array lightingColorAttachments = { lightAttachmentReference };

    subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[1].inputAttachmentCount = (uint32_t)lightingInputAttachments.size();
    subpasses[1].pInputAttachments = lightingInputAttachments.data();
    subpasses[1].colorAttachmentCount = (uint32_t)lightingColorAttachments.size();
    subpasses[1].pColorAttachments = lightingColorAttachments.data();
    subpasses[1].pResolveAttachments = nullptr;
    subpasses[1].pDepthStencilAttachment = nullptr;
    subpasses[1].preserveAttachmentCount = 0;
    subpasses[1].pPreserveAttachments = nullptr;

    // Composite Subpass
    std::array compositeInputAttachments = {  };

    std::array<VkSubpassDependency, 1> dependencies;

    // Lighting Subpass <depends on> Geometry Subpass Depth Image
    dependencies[0].srcSubpass = 0;
    dependencies[0].dstSubpass = 1;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
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

DescriptorSetCache* Renderer::GetDescriptorSetCache() {
    return &_descriptorSetCache;
}

} // namespace bl
