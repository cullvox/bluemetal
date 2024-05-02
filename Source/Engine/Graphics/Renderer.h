#pragma once

#include "Device.h"
#include "Image.h"
#include "Pipeline.h"
#include "Swapchain.h"
#include "PresentPass.h"
#include "Config.h"

namespace bl {

class Renderer
{
public:
    Renderer(Device* device, Window* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    void Recreate();
    void Render(std::function<void(VkCommandBuffer)> func);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();

    Device* _device;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    /// std::shared_ptr<GeometryPass> _geometryPass;
    std::unique_ptr<PresentPass> _presentPass;
};

} // namespace bl
