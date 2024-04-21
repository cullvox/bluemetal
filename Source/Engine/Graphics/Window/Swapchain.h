#pragma once

#include "Graphics/Device.h"
#include "Window.h"

namespace bl
{

/** @brief Swap present images for rendering multiple frames at a time. */
class Swapchain : public NonCopyable
{
public:
    Swapchain(Device& device, Window& window); /** @brief Constructor */
    ~Swapchain(); /** @brief Destructor */

    VkSwapchainKHR Get() const; /** @brief Returns the swapchain object. */
    VkFormat GetFormat() const; /** @brief Returns the format that the swapchain images are using. */
    VkExtent2D GetExtent() const; /** @brief Returns the pixel extent of the swapchain. */
    uint32_t GetImageCount() const; /** @brief Returns the number of images that the swapchain is swapping between. */
    VkPresentModeKHR GetPresentMode() const; /** @brief Returns the present mode the swapchain is using. */
    VkSurfaceFormatKHR GetSurfaceFormat() const; /** @brief Returns the surface format the swapchain is using. */
    std::vector<VkImage> GetImages() const; /** @brief Returns the handles to images that are being swapped. */
    std::vector<VkImageView> GetImageViews() const; /** @brief Returns handles to image views of the swapchain images from @ref getImages. */
    uint32_t GetImageIndex() const; /** @brief Returns the index of the swapchain image to render to. */
    bool GetMailboxSupported() const; /** @brief Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. */
    bool GetImmediateSupported() const; /** @brief Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device. */
    bool AcquireNext(VkSemaphore semaphore, VkFence fence); /** @brief Gets the next image in the chain. Returns true if the swapchain needed to be recreated, you'll have to recreate your framebuffers and other accessories. */
    void QueuePresent(VkSemaphore semaphore); /** Presents the image at GetImageIndex() to the screen. */
    void Recreate(std::optional<VkPresentModeKHR> presentMode = std::nullopt, std::optional<VkSurfaceFormatKHR> surfaceFormat = std::nullopt); /** @brief Forces a swapchain recreation. */

private:  
    void EnsureSurfaceSupported(); /** @brief Throws if a surface isn't supported for some strange reason. */
    void ChooseImageCount(); /** @brief Returns true if an image count was chosen successfully. */
    void ChooseFormat(); /** @brief Returns true if the surface format was properly found. */
    void ChoosePresentMode(); /** @brief Returns true if the present mode was properly found. */
    void ChooseExtent(); /** @brief Returns true if an extent was found. */
    void ObtainImages(); /** @brief Returns true if swapchain images could be obtained. */
    void CreateImageViews(); /** @brief Creates image views for frame buffers. */
    void DestroyImageViews(); /** @brief Destroys all the swapchain's image views. */
    
    Device& _device;
    PhysicalDevice& _physicalDevice;
    Window& _window;
    uint32_t _imageCount;
    VkSurfaceFormatKHR _surfaceFormat;
    VkPresentModeKHR _presentMode;
    VkExtent2D _extent;
    VkSwapchainKHR _swapchain;
    std::vector<VkImage> _swapImages;
    std::vector<VkImageView> _swapImageViews;
    uint32_t _imageIndex;
    bool _isMailboxSupported;
    bool _isImmediateSupported;
};

} // namespace bl
