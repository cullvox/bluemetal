#pragma once

#include "Window/Window.h"
#include "Device.h"

namespace bl
{

/** @brief Swap present images for rendering multiple frames at a time. */
class BLUEMETAL_API GfxSwapchain {
public:

    /** @brief Default format to look for. */
    static inline constexpr VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    /** @brief Default color space to look for. */
    static inline constexpr VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    /** @brief Default surface format to look for if user left unspecified. 
     * Will use index 0 of @ref GfxPhysicalDevice::getSurfaceFormats if this is unavailable. */
    static inline constexpr VkSurfaceFormatKHR DEFAULT_SURFACE_FORMAT = VkSurfaceFormatKHR{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /** @brief Default present mode to use if user left unspecified. Will use present mode FIFO if this is unavailable. */
    static inline constexpr VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    /** @brief Constructor */
    GfxSwapchain(
        std::shared_ptr<GfxDevice>          device, 
        std::shared_ptr<Window>             window,                         /** @brief The window this swapchain is swapping onto. */
        std::optional<VkPresentModeKHR>     presentMode = std::nullopt,     /** @brief Method of presentation onto the screen. */
        std::optional<VkSurfaceFormatKHR>   surfaceFormat = std::nullopt);  /** @brief Color and image format used to create swapchain images. */

    /** @brief Destructor */
    ~GfxSwapchain();

public:

    /** @brief Returns the format that the swapchain images are using. */
    VkFormat getFormat() const;

    /** @brief Returns the pixel extent of the swapchain. */
    VkExtent2D getExtent() const;

    /** @brief Returns the number of images that the swapchain is swapping between. */
    uint32_t getImageCount() const;

    /** @brief Returns the present mode the swapchain is using. */
    VkPresentModeKHR getPresentMode() const;

    /** @brief Returns the surface format the swapchain is using. */
    VkSurfaceFormatKHR getSurfaceFormat() const;

    /** @brief Returns the handles to images that are being swapped. */
    std::vector<VkImage> getImages() const;

    /** @brief Returns handles to image views of the swapchain images from @ref getImages. */
    std::vector<VkImageView> getImageViews() const;

    /** @brief Returns the swapchain object. */
    VkSwapchainKHR get() const;

    /** @brief Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. */
    bool isMailboxSupported() const { return _isMailboxSupported; }

    /** @brief Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device. */
    bool isImmediateSupported() const { return _isImmediateSupported; }

    /** @brief Gets the index to the next image being used in the swapchain.
     *
     *  @param semaphore      Semaphore to signal when the image is usable.
     *  @param fence          Fence to signal when presenting is complete.
     *  @param pImageIndex    The next image index being used in @ref getImages(). 
     *  @param pRecreated     If the swapchain was recreated the render pass might need to recreate some images. (e.g. Depth Buffer)
     */
    void acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& bRecreated);

    /** @brief Destroys and recreates the swapchain, changes present mode.
     *
     *  @param presentMode What present mode to use when swapping, use @ref PhysicalDevice::getPresentModes to get them.
     */
    void recreate(std::optional<VkPresentModeKHR> presentMode = std::nullopt);

private:  

    /** @brief Returns true if the window's VkSurfaceKHR is supported by the VkPhysicalDevice. */
    void ensureSurfaceSupported();

    /** @brief Returns true if an image count was chosen successfully. */
    void chooseImageCount();

    /** @brief Returns true if the surface format was properly found. */
    void chooseFormat(std::optional<VkSurfaceFormatKHR> surfaceFormat = std::nullopt);

    /** @brief Returns true if the present mode was properly found. */
    void choosePresentMode(std::optional<VkPresentModeKHR> presentMode = std::nullopt);

    /** @brief Returns true if an extent was found. */
    void chooseExtent();

    /** @brief Returns true if swapchain images could be obtained. */
    void obtainImages();

    /** @brief Creates image views for frame buffers. */
    void createImageViews();
    void destroyImageViews();
    
    std::shared_ptr<GfxPhysicalDevice>  _physicalDevice;
    std::shared_ptr<GfxDevice>          _device;
    std::shared_ptr<Window>             _window;
    uint32_t                            _imageCount;
    VkSurfaceFormatKHR                  _surfaceFormat;
    VkPresentModeKHR                    _presentMode;
    VkExtent2D                          _extent;
    VkSwapchainKHR                      _swapchain;
    std::vector<VkImage>                _swapImages;
    std::vector<VkImageView>            _swapImageViews;
    bool                                _isMailboxSupported;
    bool                                _isImmediateSupported;
};

} // namespace bl
