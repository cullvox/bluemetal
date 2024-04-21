#pragma once

#include "Device.h"
#include "Image.h"
#include "Pipeline.h"
#include "Swapchain.h"
#include "GeometryPass.h"
#include "PresentPass.h"
#include "GraphicsLimits.h"

namespace bl {

class BLUEMETAL_API Renderer
{
public:

    /** @brief Create Constructor */
    Renderer(
        std::shared_ptr<GfxDevice> device,     /** @brief Graphics device to use when making the renderer. */
        std::shared_ptr<Swapchain> swapchain); /** @brief Swapchain to use when presenting. */
    
    /** @brief Default Destructor */
    ~Renderer();

public:
    
    std::shared_ptr<GeometryPass> getGeometryPass();
    std::shared_ptr<RenderPass> getUserInterfacePass();
    std::shared_ptr<PresentPass> getPresentPass();

    void resize(VkExtent2D extent);
    void render(std::function<void(VkCommandBuffer)> func);

private:
    void createSyncObjects();
    void destroySyncObjects();

    std::shared_ptr<GfxDevice>          _device;
    std::shared_ptr<Swapchain>          _swapchain;
    uint32_t                            _currentFrame;
    std::vector<VkCommandBuffer>        _swapCommandBuffers;
    std::vector<VkSemaphore>            _imageAvailableSemaphores;
    std::vector<VkSemaphore>            _renderFinishedSemaphores;
    std::vector<VkFence>                _inFlightFences;
    std::shared_ptr<GeometryPass>       _geometryPass;
    std::shared_ptr<PresentPass>        _presentPass;
};

} // namespace bl
