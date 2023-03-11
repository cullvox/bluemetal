#pragma once

#include "Core/Export.h"
#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

namespace bl 
{

/** 
* \brief Creates and manages a vulkan swapchain instance.
* 
* This class manages the Vulkan swapchain from start of litefime 
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

    /** \brief The default requested format for the swapchain. */
    const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    /** \brief The default requested color space for the swapchain. */
    const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    
    /** \brief The default requested present mode for the swapchain. */
    const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    /** \brief Constructs a Vulkan swapchain.
    * 
    * Creates a VkSurfaceKHR using the function defined by the window. Then 
    * uses the surface to build data about how the swapchain will work.
    * Finally creates the swapchain using \ref recreate.
    * 
    * After this constructor is used you MUST check if the swapchain was
    * created successfully by using \ref good. If you don't the swapchain
    * will fail when using \ref acquireNext. This is purely for safety
    * reasons and not to be a pain and since this project likes to use noexcept.
    * 
    * 
    * \param window The window the swapchain is built on.
    * \param renderDevice Uses the vulkan device to create the swapchain.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa good
    * \sa recreate
    * \sa Window::createVulkanSurface 
    * 
    */
    Swapchain(Window& window, RenderDevice& renderDevice) noexcept;

    /** \brief Destructs the Vulkan swapchain.
    * 
    * Destroys the whole swapchain including the surface, and the swapchain.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Swapchain::Swapchain
    */
    ~Swapchain() noexcept;

    /** \brief Checks if the swapchain creation was successful.
    * 
    * This function checks if the value of the swapchain for a VK_NULL_HANDLE.
    * If the swapchain was not created properly it should be a null handle and
    * you will need to recreate it before using this swapchain.
    * 
    * 
    * \return true The swapchain was created successfully.
    *         false The sawpchain was not created successfully.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    *
    */
    [[nodiscard]] bool good() const noexcept;

    /** \brief Gets the internal vulkan swapchain.
    * 
    * Gets the swapchain that was created by \ref recreate. If the swapchain
    * was improperly created this function will return VK_NULL_HANDLE.
    * 
    * \return The swapchain or VK_NULL_HANDLE.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    */
    [[nodiscard]] VkSwapchainKHR getSwapchain() const noexcept;

    /** \brief Gets the internal vulkan surface.
    * 
    * Gets the surface created by the class at construction. If the surface
    * creation was not good this function will return VK_NULL_HANDLE.
    * 
    * \return The surface or VK_NULL_HANDLE.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa Swapchain()
    *
    */
    [[nodiscard]] VkSurfaceKHR getSurface() const noexcept;

    /** \brief Gets the internal swapchain image color format.
    * 
    * Gets the current color format that the swapchain used to create the 
    * images with. The format retrieved choosen by \ref recreate. If the
    * format could not be found or the swapchain was not created this function
    * will return VK_FORMAT_UNKNOWN.
    * 
    * \return The format or VK_FORMAT_UNKNOWN.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * 
    */
    [[nodiscard]] VkFormat getFormat() const noexcept;

    /** \brief Gets the extent that the swapchain is currently using.
    * 
    * Gets the extent choosen by the swapchain and is currently using. This 
    * value will only change when the swapchain is recreated and isn't
    * directly tied to the windows size.
    * 
    * \return The current swapchain extent.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * 
    */
    [[nodiscard]] Extent2D getExtent() const noexcept;

    /** \brief Gets the amount of images used by the swapchain.
    * 
    * This function returns the image count that the swapchain is currently
    * using. It only changes when \ref recreate is called. Getting the size of
    * the value returned from \ref getImages is functionally the same.
    *
    * \return Returns the image choosen image count by the swapchain when using
    * \ref recreate.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa recreate
    * \sa getImages
    * 
    */
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
    bool obtainImages() noexcept;
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

#ifdef BLOODLUST_DEBUG
    bool m_checkedGood;
#endif

};

} /* namespace bl */