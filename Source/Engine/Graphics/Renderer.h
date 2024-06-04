#pragma once

#include "Config.h"
#include "Device.h"
#include "Window.h"
#include "PresentPass.h"

namespace bl {

class Renderer
{
public:
    Renderer(Device* device, Window* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    Window* GetWindow();
    void Recreate();
    RenderPass* GetUIPass();
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::numFramesInFlight - 1. */
    
    void Render(std::function<void(VkCommandBuffer cmd, uint32_t currentFrame)> func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();

    Device* _device;
    Window* _window;
    Swapchain* _swapchain;
    uint32_t _currentFrame;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    /// std::shared_ptr<GeometryPass> _geometryPass;
    std::unique_ptr<PresentPass> _presentPass;
};

} // namespace bl
