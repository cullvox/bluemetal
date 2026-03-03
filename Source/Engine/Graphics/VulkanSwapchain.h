#pragma once

#include <optional>
#include <vector>
#include <array>

#include "VulkanDevice.h"
#include "Window/Window.h"

namespace bl {

/// @brief Swap present images for rendering multiple frames at a time.
class VulkanSwapchain {
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

    void CreateSyncObjects();
    void DestroySyncObjects();

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
    bool _isMailboxSupported;
    bool _isImmediateSupported;

public:
    /**
     * @brief Default constructor for VulkanSwapchain.
     */
    VulkanSwapchain();

    /**
     * @brief Move constructor for VulkanSwapchain.
     * @param other The VulkanSwapchain to move from.
     */
    VulkanSwapchain(VulkanSwapchain&& other);

    /**
     * @brief Constructs a VulkanSwapchain with the specified device and window.
     * @param device The Vulkan device to use.
     * @param window The Vulkan window to create the swapchain for.
     */
    VulkanSwapchain(VulkanDevice* device, VulkanWindow* window);

    /**
     * @brief Destructor for VulkanSwapchain.
     */
    ~VulkanSwapchain();

    /**
     * @brief Returns the format that the swapchain images are using.
     * @return The format of the swapchain images.
     */
    VkFormat GetFormat() const;

    /**
     * @brief Returns the pixel extent of the swapchain.
     * @return The extent of the swapchain in pixels.
     *
     * This is the size of the images that are being swapped. Use this to set the viewport size.
     */
    VkExtent2D GetExtent() const;

    /**
     * @brief Returns the number of images in the swapchain.
     * @return The number of images in the swapchain.
     */
    uint32_t GetImageCount() const;

    /**
     * @brief Returns the present mode of the swapchain.
     * @return The present mode of the swapchain.
     */
    VkPresentModeKHR GetPresentMode() const;

    /**
     * @brief Returns the surface format of the swapchain.
     * @return The surface format of the swapchain.
     */
    VkSurfaceFormatKHR GetSurfaceFormat() const;

    /**
     * @brief Returns the images that are being swapped in the swapchain.
     * @return A vector of VkImage handles representing the swapchain images.
     */
    std::vector<VkImage> GetImages() const;

    /**
     * @brief Returns the image views of the swapchain images.
     * @return A vector of VkImageView handles for the swapchain images.
     */
    std::vector<VkImageView> GetImageViews() const;

    /**
     * @brief Returns the swapchain object.
     * @return The VkSwapchainKHR handle for the swapchain.
     */
    VkSwapchainKHR Get() const;

    bool GetMailboxSupported() const; /** @brief Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. */
    bool GetImmediateSupported() const; /** @brief Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device. */

    void SetSurfaceFormat(VkSurfaceFormatKHR format);
    void SetPresentMode(VkPresentModeKHR mode);
    void Recreate(std::optional<VkPresentModeKHR> presentMode = {}, std::optional<VkSurfaceFormatKHR> surfaceFormat = {});
    bool AcquireNext(uint32_t& imageIndex, VkSemaphore imageAvailableSemaphore);
    void QueueSubmit(VkCommandBuffer cmd, VkPipelineStageFlags waitDstStageMask);
    bool QueuePresent(uint32_t imageIndex, std::span<VkSemaphore> waitSemaphores); /** Presents the image at GetImageIndex() to the screen. */
    void Destroy();
};

} // namespace bl
