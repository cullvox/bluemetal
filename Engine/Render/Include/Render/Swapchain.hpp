#pragma once

#include "Core/Export.h"
#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl 
{

/** 
* \brief Creates a Vulkan swapchain to get images from.
* 
* \description This class manages the Vulkan swapchain from start of litefime 
* to the end of its lifetime. It will create a surface for the specified 
* window. It will recreate the swapchain when acquiring an image fails to 
* create. Renderers can get the images and do their own processing.
* 
* Requires a window an render device in order to be created.
* 
* You can create multiple swapchains for multiple windows if needed.
* 
* \since BloodLust 1.0.0
* 
* \sa Window
* \sa RenderDevice
*
*/
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

    /// @brief Gets the surface created by this swapchain.
    /// @return Surface
    [[nodiscard]] VkSurfaceKHR getSurface() const noexcept;

    /// @brief Gets the current color format that the swapchain used to create the images with.
    /// @return The swap image format.
    [[nodiscard]] VkFormat getFormat() const noexcept;

    /// @brief Gets the extent that the swapchain is currently using.
    /// @return Extent2D The current swapchain extent, changed by rebuild().
    [[nodiscard]] Extent2D getExtent() const noexcept;

    /// @brief Gets the amount of images in swap.
    /// @return uint32_t Image count
    [[nodiscard]] uint32_t getImageCount() const noexcept;
    
    /// @brief Gets the vulkan images that are being used in the swapchain.
    /// @return const std::vector<VkImage>& The images.
    [[nodiscard]] const std::vector<VkImage>& getSwapchainImages() const noexcept;

    /// @brief Gets the next image in the chain.
    /// @param semaphore Semaphore to signal.
    /// @param fence Fence to trigger when done.
    /// @param imageIndex The current image index we are using. @see getSwapchainImages
    /// @return True on image acquired and swapchain successfully recreated, false on image not acquired or swapchain failed to recreate.
    [[nodiscard]] bool acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex) noexcept;

    /// @brief Recreates the swapchain on demand.
    /// @return True on success, false on failure.
    [[nodiscard]] bool recreate();

private:
    
    /// @brief Checks if the created surface supports the physical device.
    /// @return True if supported, false if not.
    [[nodiscard]] bool isSurfaceSupported() noexcept;

    /// @brief Chooses how many images the swapchain will use.
    /// @return True on success, false on failure.
    [[nodiscard]] bool chooseImageCount() noexcept;

    /// @brief Chooses the format the swapchain images will be in, requests default and uses first one if not found.
    /// @return True on success, false on failure.
    [[nodiscard]] bool chooseFormat() noexcept;

    /// @brief Chooses the present mode of the swapchain, requests default and if not uses FIFO.
    /// @return True on success, false on failure.
    bool choosePresentMode() noexcept;

    /// @brief Chooses the image extent, usually goes with swapchain capabilites, sometimes with the window size.
    /// @return True on success, false on failure.
    bool chooseExtent() noexcept;

    ///
    bool queryImages() noexcept;
    void destroy() noexcept;

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