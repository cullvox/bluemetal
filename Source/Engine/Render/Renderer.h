#pragma once

#include "Graphics/Device.h"
#include "Graphics/Image.h"
#include "Graphics/Swapchain.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderPass.h"
#include "PresentRenderPass.h"

namespace bl
{

class BLUEMETAL_API Renderer
{
    static const inline uint32_t maxFramesInFlight = 2;

public:
    Renderer(std::shared_ptr<Device> device, std::shared_ptr<Swapchain> swapchain);
    ~Renderer();

    std::shared_ptr<RenderPass> getImGuiPass();

    void resize(VkExtent2D extent);
    void render();
private:
    void createSyncObjects();
    void destroySyncObjects() noexcept;

    std::shared_ptr<Device>             m_device;
    std::shared_ptr<Swapchain>          m_swapchain;
    uint32_t                            m_currentFrame;
    std::vector<VkCommandBuffer>        m_swapCommandBuffers;
    std::vector<VkSemaphore>            m_imageAvailableSemaphores;
    std::vector<VkSemaphore>            m_renderFinishedSemaphores;
    std::vector<VkFence>                m_inFlightFences;
    std::shared_ptr<PresentRenderPass>  m_presentPass;
};

} // namespace bl