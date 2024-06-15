#pragma once

#include "Config.h"
#include "Device.h"
#include "Graphics/DescriptorSetCache.h"
#include "Window.h"
#include "GeometryPass.h"
#include "PresentPass.h"
#include "vulkan/vulkan_core.h"

namespace bl {

using RenderFunction = std::function<void(VkCommandBuffer cmd, uint32_t currentFrame)>;

class Renderer {
public:
    Renderer(Device* device, Window* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    Window* GetWindow();
    VkRenderPass* GetRenderPass();
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::numFramesInFlight - 1. */
    DescriptorSetCache* GetDescriptorSetCache();
    void Render(RenderFunction func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();
    void CreateRenderPass();
    void DestroyRenderPass();
    void DestroyImagesAndFramebuffers();

    Device* _device;
    Window* _window;
    Swapchain* _swapchain;

    // Frame Synchronization
    uint32_t _currentFrame;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;

    // Render Pass Data
    VkFormat _lightFormat, _depthFormat, _albedoFormat, _normalFormat;
    VkRenderPass _pass;
    std::vector<std::unique_ptr<Image>> _lightImages, _depthImages, _albedoImages, _normalImages;
    std::vector<VkFramebuffer> _framebuffers;

    DescriptorSetCache _descriptorSetCache;
};

} // namespace bl
