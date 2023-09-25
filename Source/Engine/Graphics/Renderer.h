#pragma once

#include "Device.h"
#include "Image.h"
#include "Pipeline.h"
#include "Swapchain.h"
#include "GeometryPass.h"
#include "PresentPass.h"

namespace bl {

struct RendererCreateInfo {
    GraphicsDevice*    pDevice;       /** @brief Graphics device to use when making the renderer. */
    Swapchain*         pSwapchain;    /** @brief Swapchain to use when presenting. */
};

class BLUEMETAL_API Renderer {
public:

    /** @brief Default Constructor */
    Renderer();

    /** @brief Move Constructor */
    Renderer(Renderer&& rhs);

    /** @brief Create Constructor */
    Renderer(const RendererCreateInfo& info);

    /** @brief Default Destructor */
    ~Renderer();

    /** @brief Create a renderer instance. */
    [[nodiscard]] bool create(const RendererCreateInfo& info) noexcept;
    
    /** @brief Destroys this renderer object. */
    void destroy() noexcept;

public:
    
    GeometryPass* getGeometryPass();
    RenderPass* getUserInterfacePass();
    PresentPass* getPresentPass();

    [[nodiscard]] bool resize(VkExtent2D extent);
    [[nodiscard]] bool render(std::function<void(VkCommandBuffer)> func);

private:

    static const inline uint32_t maxFramesInFlight = 2;

    bool createSyncObjects();
    void destroySyncObjects() noexcept;

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
