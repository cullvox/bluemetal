#pragma once

//===========================//
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"
//===========================//

namespace bl {

class Swapchain {
public:
    static inline const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
    static inline const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    CSwapchain(CConfig* pConfig, CRenderDevice* pRenderDevice, IWindow* pWindow);
    ~CSwapchain();

    VkSwapchainKHR  GetSwapchain() const noexcept;
    VkSurfaceKHR    GetSurface() const noexcept;

    void            OnConfigReceive(const std::vector<SConfigValue>&) override;
private:
    void                EnsureSurfaceSupported();
    uint32_t            GetImageCount();
    VkSurfaceFormatKHR  FindSurfaceFormat();
    VkPresentModeKHR    FindPresentMode();

    RenderDevice&       m_renderDevice;
    Window&             m_window;
    uint32_t            m_imageCount;
    VkSurfaceFormatKHR  m_surfaceFormat;
    VkPresentModeKHR    m_presentMode;
    VkSwapchainKHR      m_swapchain;
};

} /* namespace bl */