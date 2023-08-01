#pragma once

#include "Math/Vector2.h"
#include "Window/Window.h"
#include "Device.h"

namespace bl
{

/// @brief 
class BLUEMETAL_API Swapchain
{
public:

    /// Default format to look for.
    static inline constexpr VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    /// Default color space to look for.
    static inline constexpr VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    /// Default surface format to look for if user left unspecified. Will use index 0 of @ref PhysicalDevice::getSurfaceFormats if this is unavailable.
    static inline constexpr VkSurfaceFormatKHR DEFAULT_SURFACE_FORMAT = VkSurfaceFormatKHR{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /// Default present mode to use if user left unspecified. Will use present mode FIFO if this is unavailable.
    static inline constexpr VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    /// Constructs a swapchain object.
    ///
    ///     @param device Logical device used to create the swapchain.
    ///     @param window The window this swapchain is swapping onto.
    ///     @param presentMode Method of presentation onto the screen.
    ///     @param surfaceFormat Color and image format used to create swapchain images.
    ///
    Swapchain(
        std::shared_ptr<Device>             device, 
        std::shared_ptr<Window>             window, 
        std::optional<VkPresentModeKHR>     presentMode = std::nullopt,
        std::optional<VkSurfaceFormatKHR>   surfaceFormat = std::nullopt);

    /// Default destructor.
    ~Swapchain();

    /// Returns the format that the swapchain images are using.
    VkFormat getFormat();

    /// Returns the pixel extent of the swapchain.
    VkExtent2D getExtent();

    /// Returns the number of images that the swapchain is swapping between.
    uint32_t getImageCount();

    /// Returns the present mode the swapchain is using.
    VkPresentModeKHR getPresentMode();

    /// Returns the surface format the swapchain is using.
    VkSurfaceFormatKHR getSurfaceFormat();

    /// Returns the handles to images that are being swapped.
    std::vector<VkImage> getImages();

    /// Returns handles to image views of the swapchain images from @ref getImages.
    ///
    /// These images are in the same order from @ref getImages.
    /// Meaning that an image at index 0 from @ref getImages, to get it's image view
    /// using @ref getImageViews would also be index 0.
    std::vector<VkImageView> getImageViews();

    /// Returns handles to framebuffers of images that are being swapped.
    ///
    /// These framebuffers are in the same order from @ref getImageViews and @getImages.
    /// Same as @ref getImageViews, the indices match up with one another,
    /// image 0, image view 0, and framebuffer 0 all relate to each other.
    std::vector<VkFramebuffer> getFramebuffers();

    /// Returns the swapchain object.
    VkSwapchainKHR getHandle();

    /// Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. 
    bool isMailboxSupported() { return m_isMailboxSupported; }

    /// Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device.
    bool isImmediateSupported() { return m_isImmediateSupported; }

    /// Gets the index to the next image being used in the swapchain.
    ///
    ///     @param semaphore Semaphore to signal when the image is usable.
    ///     @param fence Fence to signal when presenting is complete.
    ///     @param pImageIndex The next image index being used in @ref getImages(). 
    ///     @param pRecreated If the swapchain was recreated the render pass might need to recreate images. (e.g. Depth Buffer)
    ///
    void acquireNext(
        VkSemaphore     semaphore, 
        VkFence         fence, 
        uint32_t*       pImageIndex, 
        bool*           pRecreated);

    /// Destroys and recreates the swapchain, changes present mode.
    ///
    ///     @param presentMode What present mode the swapchain is using, FIFO is VSync. Use @ref PhysicalDevice::getPresentModes to get them.
    ///
    void recreate(
        std::optional<VkPresentModeKHR> presentMode);
        
    /// Destroy and creates the swapchain without changing any parameters.
    void recreate();

private:  
    void ensureSurfaceSupported();
    void chooseImageCount();
    void chooseFormat(std::optional<VkSurfaceFormatKHR> surfaceFormat);
    void choosePresentMode(std::optional<VkPresentModeKHR> presentMode);
    void chooseExtent();
    void obtainImages();
    void createImageViews();
    void destroyImageViews();
    
    std::shared_ptr<PhysicalDevice> m_physicalDevice;
    std::shared_ptr<Device>         m_device;
    std::shared_ptr<Window>         m_window;
    uint32_t                        m_imageCount;
    VkSurfaceFormatKHR              m_surfaceFormat;
    VkPresentModeKHR                m_presentMode;
    VkExtent2D                      m_extent;
    VkSwapchainKHR                  m_swapchain;
    std::vector<VkImage>            m_swapImages;
    std::vector<VkImageView>        m_swapImageViews;
    bool                            m_isMailboxSupported;
    bool                            m_isImmediateSupported;
};

} // namespace bl