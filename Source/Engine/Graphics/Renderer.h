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
    void Render(std::function<void(VkCommandBuffer)> func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();

    Device* _device;
    Window* _window;
    Swapchain* _swapchain;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    /// std::shared_ptr<GeometryPass> _geometryPass;
    std::unique_ptr<PresentPass> _presentPass;
};

} // namespace bl
