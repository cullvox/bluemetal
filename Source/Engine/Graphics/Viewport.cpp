#include "Viewport.h"

#include "RenderData.h"
#include "RenderPass.h"
#include "VulkanSwapchain.h"
#include "RenderData.h"
#include "UniformData.h"
#include "Renderer.h"
#include "VulkanDescriptorSetAllocatorCache.h"

namespace bl {


bool Viewport::UseSwapchain() { 
    return _swapchain != nullptr; 
}


Viewport::Viewport(Renderer* renderer, VulkanDevice* device, VkExtent2D extent)
    : _renderer(renderer)
    , _device(device)
    , _swapchain(nullptr)
    , _imageAvailableSemaphores({})
    , _renderFinishedSemaphores({})
    , _sampleCount(VK_SAMPLE_COUNT_1_BIT)
{

    // Create the viewport descriptor sets to bind later.
    std::vector<VkDescriptorSetLayoutBinding> bindings { 1 };
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    _globalDescriptorSetLayout = _device->AcquireDescriptorSetLayout(bindings);
    _globalBuffer = VulkanBufferFrameRing{ _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::GlobalUBO) };

    std::array<VkDescriptorBufferInfo, VulkanConfig::maxFramesInFlight> descriptorBufferInfos;
    std::array<VkWriteDescriptorSet, VulkanConfig::maxFramesInFlight> descriptorWrites;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _globalDescriptorSets[i] = _renderer->GetDescriptorSetAllocatorCache()->Allocate(_globalDescriptorSetLayout);

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

    // Create the viewport images.
    RecreateImages();

}

Viewport::Viewport(Renderer* renderer, VulkanDevice* device, VulkanSwapchain* swapchain)
    : _swapchain(swapchain)
{
    Viewport(renderer, device, swapchain->GetExtent());

    // Build out the per-frame semaphores infos.
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    // Create the semaphores for each frame in flight.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) 
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]))
    }

    // Create a render-finished semaphore for each frame in the synchronization driver.
    uint32_t imageCount = _swapchain->GetImageCount();
    _renderFinishedSemaphores.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++)
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
    }
}

Viewport::~Viewport()
{

    // Free the viewport global descriptor sets.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _renderer->GetDescriptorSetAllocatorCache()->Free(_globalDescriptorSetLayout, _globalDescriptorSets[i]);
    }

    // If not using a swapchain, no semaphores are used and do not need to be freed.
    if (!_swapchain)
    {
        return;
    }

    // Destroy all per-frame in flight semaphores.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        vkDestroySemaphore(_device->Get(), _imageAvailableSemaphores[i], nullptr);
    }

    // Destroy all per-image render finished semaphores. 
    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++)
    {
        vkDestroySemaphore(_device->Get(), _renderFinishedSemaphores[i], nullptr);
    }
}

void Viewport::Render(RenderData& renderData, VkSemaphoreSubmitInfo& waitInfo, VkSemaphoreSubmitInfo& signalInfo)
{

    uint32_t imageIndex = 0;

    // Acquire the next image from the swapchain.
    if (_swapchain) {
        if (_swapchain->AcquireNext(imageIndex, _imageAvailableSemaphores[renderData.GetCurrentFrame()])) {
            RecreateImages(_swapchain->GetExtent());
            return; // This frame will not be rendered.
        }
    }

    // Update the viewport uniform buffer.
    const auto extent = ;

    _uboData.time = _renderer->GetCurrentFrameTime();
    _uboData.dt = _renderer->GetCurrentFrameDeltaTime();
    _uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    _uboData.mouse = {}; // TODO: mouse position to be added later.

    auto& ubo = vp.guboData.globalBuffer;
    ubo.Upload(std::as_bytes(std::span<GlobalUBO, 1>{&_uboData, 1}), _currentFrame);

    for (auto& pass : _renderPasses) {
        pass->Render(*this, renderData);
    }

    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.semaphore = _imageAvailableSemaphores[renderData.GetCurrentFrame()];
    waitInfo.value = 0;
    waitInfo.stageMask = 0;
    waitInfo.deviceIndex = 0;

    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalInfo.pNext = nullptr;
    signalInfo.semaphore = _imageAvailableSemaphores[renderData.GetCurrentFrame()];
    signalInfo.value = 0;
    signalInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    signalInfo.deviceIndex = 0;
}



}