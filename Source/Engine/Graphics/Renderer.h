#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Device.h"
#include "Image.h"
#include "Swapchain.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "PresentRenderPass.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

class BLUEMETAL_API Renderer
{
    static const inline uint32_t maxFramesInFlight = 2;

public:
    Renderer();
    ~Renderer();

    bool create(GraphicsDevice& device, Swapchain& swapchain);

    RenderPass& getGeometryPass();
    RenderPass& getUserInterfacePass();

    void resize(VkExtent2D extent);
    void render(std::function<void(VkCommandBuffer)> func);

private:
    void createSyncObjects();
    void destroySyncObjects() noexcept;

    GraphicsDevice&                     m_device;
    Swapchain&                          m_swapchain;
    uint32_t                            m_currentFrame;
    std::vector<VkCommandBuffer>        m_swapCommandBuffers;
    std::vector<VkSemaphore>            m_imageAvailableSemaphores;
    std::vector<VkSemaphore>            m_renderFinishedSemaphores;
    std::vector<VkFence>                m_inFlightFences;

    GeometryPass                        m_geometryPass;
    PresentPass                         m_presentPass;
};

} // namespace bl