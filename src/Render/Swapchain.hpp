#pragma once

#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl {

class Swapchain {
public:
    static inline const VkFormat            DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
    static inline const VkColorSpaceKHR     DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    static inline const VkPresentModeKHR    DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    Swapchain(Window& window, RenderDevice& renderDevice);
    ~Swapchain();

    VkSwapchainKHR getSwapchain() const noexcept;
    VkSurfaceKHR getSurface() const noexcept;
    VkFormat getColorFormat() const noexcept;
    Extent2D getSwapchainExtent();
    uint32_t getImageCount() const noexcept;
    const std::vector<VkImage>& getSwapchainImages() const noexcept;
    void acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& wasRecreated);
private:
    void ensureSurfaceSupported();
    void findImageCount();
    void findSurfaceFormat();
    void findPresentMode();
    void getImages();
    void destroySwapchain();
    void recreateSwapchain();
    

    RenderDevice&       m_renderDevice;
    Window&             m_window;
    VkSurfaceKHR        m_surface;
    uint32_t            m_imageCount;
    VkSurfaceFormatKHR  m_surfaceFormat;
    VkPresentModeKHR    m_presentMode;
    VkSwapchainKHR      m_swapchain;
    bool                m_wasRecreated;
    std::vector<VkImage> m_swapImages;
    std::vector<VkCommandBuffer> m_swapBuffers;
};

} /* namespace bl */