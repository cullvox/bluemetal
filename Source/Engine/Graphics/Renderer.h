#pragma once

#include "Window/Window.h"
#include "VulkanFrameTracker.h"
#include "VulkanConfig.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanImage.h"

namespace bl {

using RenderFunction = std::function<void(VkCommandBuffer cmd, uint32_t currentFrame)>;

class Renderer {
public:
    Renderer(const VulkanDevice* device, Window* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    VkRenderPass GetRenderPass() const;
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::numFramesInFlight - 1. */
    void Render(RenderFunction func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();
    void CreateRenderPasses();
    void DestroyRenderPasses();
    void DestroyImagesAndFramebuffers();
    void RecreateImages();

    const VulkanDevice* _device;
    const VulkanWindow* _window;
    VulkanSwapchain _swapchain;

    // Frame Synchronization
    uint32_t _imageCount;
    uint32_t _currentFrame;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;

    // Render Pass Data
    VkFormat _depthFormat;
    VkRenderPass _pass;
    std::vector<VulkanImage> _depthImages;
    std::vector<VkFramebuffer> _framebuffers;

    VulkanDescriptorSetAllocatorCache _descriptorSetCache;
};

} // namespace bl
