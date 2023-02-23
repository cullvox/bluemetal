#pragma once

#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl {

class Swapchain {
public:
    static inline const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
    static inline const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    static inline const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    Swapchain(Window& window, RenderDevice& renderDevice);
    ~Swapchain();

    VkSwapchainKHR getSwapchain() const noexcept;
    VkSurfaceKHR getSurface() const noexcept;

private:
    void ensureSurfaceSupported();
    void getImageCount();
    void findSurfaceFormat();
    void findPresentMode();
    void recreateSwapchain();

    RenderDevice&       m_renderDevice;
    Window&             m_window;
    VkSurfaceKHR        m_surface;
    uint32_t            m_imageCount;
    VkSurfaceFormatKHR  m_surfaceFormat;
    VkPresentModeKHR    m_presentMode;
    VkSwapchainKHR      m_swapchain;
};

} /* namespace bl */