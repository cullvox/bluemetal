#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Window/Window.h"
#include "Vulkan.h"
#include "VulkanMutable.h"
#include "VulkanDevice.h"
#include <vulkan/vulkan_core.h>

namespace bl {

/// @brief Swap present images for rendering multiple frames at a time.
class VulkanSwapchain : public NonCopyable, public VulkanMutable {
public:

    /// @brief Default Constructor
    VulkanSwapchain();

    /// @brief Move Construtor
    VulkanSwapchain(VulkanSwapchain&& other);

    /// @brief Swapchain Constructor
    ///
    /// @brief[in] device The vulkan device to create this swapchain with.
    /// @brief[in] window A valid vulkan window.
    VulkanSwapchain(VulkanDevice* device, VulkanWindow* window);
    
    /// @brief Destructor
    ~VulkanSwapchain();

    VkFormat GetFormat() const; /** @brief Returns the format that the swapchain images are using. */
    VkExtent2D GetExtent() const; /** @brief Returns the pixel extent of the swapchain. */
    uint32_t GetImageCount() const; /** @brief Returns the number of images being swapped. */
    VkPresentModeKHR GetPresentMode() const; /** @brief Returns the present mode the swapchain is using. */
    VkSurfaceFormatKHR GetSurfaceFormat() const; /** @brief Returns the surface format the swapchain is using. */
    std::vector<VkImage> GetImages() const; /** @brief Returns the handles to images that are being swapped. */
    std::vector<VkImageView> GetImageViews() const; /** @brief Returns handles to image views of the swapchain images from @ref getImages. */
    VkSwapchainKHR Get() const; /** @brief Returns the swapchain object. */
    uint32_t GetImageIndex() const; /** @brief Returns the index of current swapchain image. */
    bool GetMailboxSupported() const; /** @brief Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. */
    bool GetImmediateSupported() const; /** @brief Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device. */

    void SetSurfaceFormat(VkSurfaceFormatKHR format);
    void SetPresentMode(VkPresentModeKHR mode);
    void Resize(VkExtent2D extent);
    void Recreate(std::optional<VkPresentModeKHR> presentMode = VK_PRESENT_MODE_FIFO_KHR, std::optional<VkSurfaceFormatKHR> surfaceFormat = {});
    bool AcquireNext(VkSemaphore semaphore, VkFence fence); 
    bool QueuePresent(VkSemaphore semaphore); /** Presents the image at GetImageIndex() to the screen. */
    void Destroy();

    virtual std::size_t GetHash() const override;

private:
    /// @brief Throws if a surface isn't supported for some strange reason.
    void EnsureSurfaceSupported();

    /// @brief Returns true if an image count was chosen successfully.
    void ChooseImageCount();

    /// @brief Returns true if the surface format was properly found.
    void ChooseFormat();

    /// @brief Returns true if the present mode was properly found.
    void ChoosePresentMode();

    /// @brief Returns true if an extent was found.
    void ChooseExtent();

    /// @brief Returns true if swapchain images could be obtained.
    void ObtainImages();

    /// @brief Creates image views for frame buffers.
    void CreateImageViews();

    /// @brief Destroys all the swapchain's image views.
    void DestroyImageViews();
    
    VulkanDevice* _device;
    VulkanPhysicalDevice* _physicalDevice;
    VulkanWindow* _window;
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
    std::size_t _hash;
};

} // namespace bl
