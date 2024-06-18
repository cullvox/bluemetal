#pragma once

#include "Window.h"
#include "VulkanConfig.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSetCache.h"
#include "VulkanImage.h"

namespace bl {

using RenderFunction = std::function<void(VkCommandBuffer cmd, uint32_t currentFrame)>;

class Renderer {
public:
    Renderer(VulkanDevice* device, VulkanWindow* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    VkRenderPass GetRenderPass() const;
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::numFramesInFlight - 1. */
    VulkanDescriptorSetCache* GetDescriptorSetCache();
    void Render(RenderFunction func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();
    void CreateRenderPasses();
    void DestroyRenderPasses();
    void DestroyImagesAndFramebuffers();
    void RecreateImages();

    VulkanDevice* _device;
    VulkanWindow* _window;
    VulkanSwapchain* _swapchain;

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

    VulkanDescriptorSetCache _descriptorSetCache;
};

} // namespace bl
