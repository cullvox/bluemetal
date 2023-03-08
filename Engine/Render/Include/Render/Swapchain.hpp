#pragma once

#include "Core/Export.h"
#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl 
{

/// @brief Creates a Vulkan swapchain to get images from.
class BLOODLUST_API Swapchain 
{
public:

    /// @brief The default requested format for the swapchain.
    const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    /// @brief The default requested color space for the swapchain.
    const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    
    /// @brief The default requested present mode for the swapchain.
    const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    /// @brief Constructs a Vulkan swapchain for a Window using the RenderDevice.
    /// @param window The window to create a swapchain for.
    /// @param renderDevice Vulkan render device to create the swapchain.
    Swapchain(Window& window, RenderDevice& renderDevice);

    /// @brief Destructs the Vulkan swapchain.
    ~Swapchain();

    /// @brief Checks if the swapchain creation was good.
    /// @return True on success, false on failure.
    [[nodiscard]] bool good() const noexcept;

    /// @brief Gets the created Vulkan swapchain
    /// @return Returns the Vulkan swapchain, VK_NULL_HANDLE on failure.
    [[nodiscard]] VkSwapchainKHR getSwapchain() const noexcept;

    /// @brief 
    [[nodiscard]] VkSurfaceKHR getSurface() const noexcept;
    [[nodiscard]] VkFormat getColorFormat() const noexcept;
    [[nodiscard]] Extent2D getSwapchainExtent() const noexcept;
    [[nodiscard]] uint32_t getImageCount() const noexcept;
    [[nodiscard]] const std::vector<VkImage>& getSwapchainImages() const noexcept;
    [[nodiscard]] bool acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& wasRecreated) noexcept;
private:
    void ensureSurfaceSupported();
    void findImageCount();
    void findSurfaceFormat();
    void findPresentMode();
    void findExtent();
    void getImages();
    void destroySwapchain();
    void recreateSwapchain();
    

    RenderDevice& m_renderDevice;
    Window& m_window;
    VkSurfaceKHR m_surface;
    uint32_t m_imageCount;
    VkSurfaceFormatKHR m_surfaceFormat;
    VkPresentModeKHR m_presentMode;
    Extent2D m_extent;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_swapImages;
    std::vector<VkCommandBuffer> m_swapBuffers;
};

} /* namespace bl */