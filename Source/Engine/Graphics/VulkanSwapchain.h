#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Graphics/Vulkan.h"

namespace bl {

class VulkanDevice;
class VulkanPhysicalDevice;
class VulkanWindow;

/** @brief Swap present images for rendering multiple frames at a time. */
class VulkanSwapchain : public NonCopyable {
public:
    VulkanSwapchain();
    VulkanSwapchain(VulkanSwapchain&& other);
    VulkanSwapchain(VulkanDevice* device, VulkanWindow* window);
    ~VulkanSwapchain();

    VkFormat GetFormat() const; /** @brief Returns the format that the swapchain images are using. */
    VkExtent2D GetExtent() const; /** @brief Returns the pixel extent of the swapchain. */
    uint32_t GetImageCount() const; /** @brief Returns the number of images being swapped. */
    VkPresentModeKHR GetPresentMode() const; /** @brief Returns the present mode the swapchain is using. */
    VkSurfaceFormatKHR GetSurfaceFormat() const; /** @brief Returns the surface format the swapchain is using. */
    VkSwapchainKHR Get() const; /** @brief Returns the swapchain object. */
    std::vector<VkImage> GetImages() const; /** @brief Returns the handles to images that are being swapped. */
    std::vector<VkImageView> GetImageViews() const; /** @brief Returns handles to image views of the swapchain images from @ref getImages. */
    bool GetMailboxSupported() const; /** @brief Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. */
    bool GetImmediateSupported() const; /** @brief Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device. */

    /** @brief Gets the next image in the chain.
     *
     *  @returns The value from WasRecreated(), if the swapchain was recreated before the acquire it will also return true as from WasRecreated().
     */
    bool AcquireNext(VkSemaphore semaphore, VkFence fence); 
    uint32_t GetImageIndex() const; /** @brief Returns the index of the swapchain image to render to. */
    bool QueuePresent(VkSemaphore semaphore); /** Presents the image at GetImageIndex() to the screen. */
    void Recreate(std::optional<VkPresentModeKHR> presentMode = std::nullopt, std::optional<VkSurfaceFormatKHR> surfaceFormat = std::nullopt); /** @brief Forces a swapchain recreation. */
    bool WasRecreated() const;

private:
    void EnsureSurfaceSupported(); /** @brief Throws if a surface isn't supported for some strange reason. */
    void ChooseImageCount(); /** @brief Returns true if an image count was chosen successfully. */
    void ChooseFormat(); /** @brief Returns true if the surface format was properly found. */
    void ChoosePresentMode(); /** @brief Returns true if the present mode was properly found. */
    void ChooseExtent(); /** @brief Returns true if an extent was found. */
    void ObtainImages(); /** @brief Returns true if swapchain images could be obtained. */
    void CreateImageViews(); /** @brief Creates image views for frame buffers. */
    void DestroyImageViews(); /** @brief Destroys all the swapchain's image views. */
    
    VulkanDevice* _device;
    const VulkanPhysicalDevice* _physicalDevice;
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
    bool _wasRecreated;
};

} // namespace bl