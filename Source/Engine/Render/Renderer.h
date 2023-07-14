#pragma once

#include "Device.h"
#include "Image.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "Pipeline.h"

class BLUEMETAL_API blRenderer
{
    static const inline uint32_t MAX_FRAMES_IN_FLIGHT = 2;

public:
    explicit blRenderer(std::shared_ptr<blDevice> device, std::shared_ptr<blSwapchain> swapchain, std::vector<std::shared_ptr<blRenderPass>>& passes);
    ~blRenderer();

    void resize(blExtent2D extent);
    void render();

private:
    void createSyncObjects();
    void destroySyncObjects() noexcept;

    std::shared_ptr<blDevice>                           _device;
    std::shared_ptr<blSwapchain> _                      _swapchain;
    const std::vector<std::shared_ptr<blRenderPass>>    _passes;  
    uint32_t                                            _currentFrame;
    std::vector<VkCommandBuffer>                        _swapCommandBuffers;
    std::vector<VkSemaphore>                            _imageAvailableSemaphores;
    std::vector<VkSemaphore>                            _renderFinishedSemaphores;
    std::vector<VkFence>                                _inFlightFences;
    
};