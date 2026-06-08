#pragma once

#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"
#include "Graphics/VulkanConfig.h"
#include "VulkanBufferFrameRing.h"

namespace bl {

class Renderer;
class VulkanDevice;
class VulkanImage;
class VulkanImageView;
class VulkanRenderImage;
class RenderData;
class RenderPass;
class VulkanSwapchain;

// Viewports manage their own frames and render images. They are responsible for presenting to the swapchain, but not for managing synchronization or determining what gets drawn.
class Viewport {

    Renderer* _renderer;
    VulkanDevice* _device;
    VkSampleCountFlagBits _sampleCount;
    std::unique_ptr<VulkanImage> _colorImage;
    std::unique_ptr<VulkanImageView> _colorImageView;
    std::vector<std::unique_ptr<RenderPass>> _renderPasses;
    VulkanBufferFrameRing _globalBuffer;
    VkDescriptorSetLayout _globalDescriptorSetLayout;
    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _globalDescriptorSets;

    VulkanSwapchain* _swapchain; // Can be nullptr if not being used.
    std::array<VkSemaphore, VulkanConfig::maxFramesInFlight> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;

    bool UseSwapchain();

public:
    Viewport(Renderer* renderer, VulkanDevice* device, VkExtent2D extent);
    Viewport(Renderer* renderer, VulkanDevice* device, VulkanSwapchain* swapchain);
    ~Viewport();

    void RecreateImages(VkExtent2D extent);

    void PreRender();
    void PostRender();

    void Render(RenderData& renderData, VkSemaphoreSubmitInfo& waitInfo, VkSemaphoreSubmitInfo& signalInfo);

};

} // namespace bl
