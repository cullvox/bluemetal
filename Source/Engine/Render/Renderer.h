#pragma once

#include "Device.h"
#include "Image.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "Pipeline.h"

namespace bl
{

class BLUEMETAL_API Renderer
{
    static const inline uint32_t MAX_FRAMES_IN_FLIGHT = 2;

public:
    Renderer(std::shared_ptr<Device> device, std::shared_ptr<Swapchain> swapchain, std::vector<std::shared_ptr<RenderPass>>& passes);
    ~Renderer();

    void resize(Extent2D extent);
    void render();

private:
    void createSyncObjects();
    void destroySyncObjects() noexcept;

    std::shared_ptr<Device>                         m_device;
    std::shared_ptr<Swapchain>                      m_swapchain;
    const std::vector<std::shared_ptr<RenderPass>>  m_passes;  
    uint32_t                                        m_currentFrame;
    std::vector<VkCommandBuffer>                    m_swapCommandBuffers;
    std::vector<VkSemaphore>                        m_imageAvailableSemaphores;
    std::vector<VkSemaphore>                        m_renderFinishedSemaphores;
    std::vector<VkFence>                            m_inFlightFences;
};

} // namespace bl