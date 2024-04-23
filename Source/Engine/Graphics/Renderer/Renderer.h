#pragma once

#include "Device.h"
#include "Image.h"
#include "Pipeline.h"
#include "Swapchain.h"
#include "GeometryPass.h"
#include "PresentPass.h"
#include "GraphicsLimits.h"

namespace bl {

class Renderer
{
public:
    Renderer(Device& device, Window& window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    void Recreate();
    void Render(std::function<void(VkCommandBuffer)> func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();

    Device& _device;
    Swapchain _swapchain;
    uint32_t _currentFrame;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    std::shared_ptr<GeometryPass> _geometryPass;
    std::shared_ptr<PresentPass> _presentPass;
};

} // namespace bl
