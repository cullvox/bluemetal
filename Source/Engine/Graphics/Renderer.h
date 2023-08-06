#pragma once

#include "Device.h"
#include "Image.h"
#include "Swapchain.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "PresentRenderPass.h"

namespace bl
{

class BLUEMETAL_API Renderer
{
    static const inline uint32_t maxFramesInFlight = 2;

public:
    Renderer(GraphicsDevice* pDevice, Swapchain* pSwapchain);
    ~Renderer();

    RenderPass* getGeometryPass();
    RenderPass* getUserInterfacePass();

    void resize(VkExtent2D extent);
    void render();
private:
    void createSyncObjects();
    void destroySyncObjects() noexcept;

    GraphicsDevice*                     m_pDevice;
    Swapchain*                          m_pSwapchain;
    uint32_t                            m_currentFrame;
    std::vector<VkCommandBuffer>        m_swapCommandBuffers;
    std::vector<VkSemaphore>            m_imageAvailableSemaphores;
    std::vector<VkSemaphore>            m_renderFinishedSemaphores;
    std::vector<VkFence>                m_inFlightFences;
    std::unique_ptr<PresentRenderPass>  m_presentPass;
};

} // namespace bl