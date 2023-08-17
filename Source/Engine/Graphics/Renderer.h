#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Device.h"
#include "Image.h"
#include "Pipeline.h"
#include "Swapchain.h"
#include "GeometryPass.h"
#include "PresentPass.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

struct RendererCreateInfo
{
    GraphicsDevice* pDevice;
    Swapchain*      pSwapchain;
};

class BLUEMETAL_API Renderer
{
    static const inline uint32_t maxFramesInFlight = 2;

public:

    // Default Constructor
    Renderer();

    // Create Constructor
    Renderer(const RendererCreateInfo& createInfo);

    // Default Destructor
    ~Renderer();

    /// Create a renderer instance.
    [[nodiscard]] bool create(const RendererCreateInfo& createInfo);
    
    /// Destroys this renderer object's internal data.
    void destroy() noexcept;

    
    RenderPass* getGeometryPass();
    RenderPass* getUserInterfacePass();

    [[nodiscard]] bool resize(VkExtent2D extent);
    [[nodiscard]] bool render(std::function<void(VkCommandBuffer)> func);

private:
    bool createSyncObjects();
    void destroySyncObjects() noexcept;

    std::string                         m_err;
    GraphicsDevice*                     m_pDevice;
    Swapchain*                          m_pSwapchain;
    uint32_t                            m_currentFrame;
    std::vector<VkCommandBuffer>        m_swapCommandBuffers;
    std::vector<VkSemaphore>            m_imageAvailableSemaphores;
    std::vector<VkSemaphore>            m_renderFinishedSemaphores;
    std::vector<VkFence>                m_inFlightFences;

    std::unique_ptr<GeometryPass>       m_geometryPass;
    std::unique_ptr<PresentPass>        m_presentPass;
};

} // namespace bl