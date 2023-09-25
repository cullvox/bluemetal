#pragma once

#include "Math/Vector2.h"
#include "Window/Window.h"
#include "Device.h"

namespace bl
{

struct SwapchainCreateInfo {
    GraphicsDevice*                   pDevice;                      /** @brief Logical device used to create the swapchain. */
    Window*                           pWindow;                      /** @brief The window this swapchain is swapping onto. */
    std::optional<VkPresentModeKHR>   presentMode = std::nullopt;   /** @brief Method of presentation onto the screen. */
    std::optional<VkSurfaceFormatKHR> surfaceFormat = std::nullopt; /** @brief Color and image format used to create swapchain images. */
};

/** @brief Swap present images for rendering multiple frames at a time. */
class BLUEMETAL_API Swapchain {
public:

    /** @brief Default format to look for. */
    static inline constexpr VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    /** @brief Default color space to look for. */
    static inline constexpr VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    /** @brief Default surface format to look for if user left unspecified. Will use index 0 of @ref PhysicalDevice::getSurfaceFormats if this is unavailable. */
    static inline constexpr VkSurfaceFormatKHR DEFAULT_SURFACE_FORMAT = VkSurfaceFormatKHR{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /** @brief Default present mode to use if user left unspecified. Will use present mode FIFO if this is unavailable. */
    static inline constexpr VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    /** @brief Default Constructor */
    Swapchain();

    /** @brief Create Constructor */
    Swapchain(const SwapchainCreateInfo& info);

    /** @brief Default destructor. */
    ~Swapchain();

    /** @brief Creates this swapchain. */
    [[nodiscard]] bool create(const SwapchainCreateInfo& info) noexcept;

    /** @brief Destroys this swapchain. */
    void destroy() noexcept;

    /** @brief Returns true if the swapchain was already created. */
    [[nodiscard]] bool isCreated() const noexcept;

public:

    /** @brief Returns the format that the swapchain images are using. */
    [[noexcept]] VkFormat getFormat() const noexcept;

    /** @brief Returns the pixel extent of the swapchain. */
    [[noexcept]] VkExtent2D getExtent() const noexcept;

    /** @brief Returns the number of images that the swapchain is swapping between. */
    [[noexcept]] uint32_t getImageCount() const noexcept;

    /** @brief Returns the present mode the swapchain is using. */
    [[noexcept]] VkPresentModeKHR getPresentMode() const noexcept;

    /** @brief Returns the surface format the swapchain is using. */
    [[noexcept]] VkSurfaceFormatKHR getSurfaceFormat() const noexcept;

    /** @brief Returns the handles to images that are being swapped. */
    [[nodiscard]] std::vector<VkImage> getImages() const noexcept;

    /** @brief Returns handles to image views of the swapchain images from @ref getImages. */
    [[nodiscard]] std::vector<VkImageView> getImageViews() const noexcept;

    /** @brief Returns handles to framebuffers of images that are being swapped from @ref getImageViews. */
    [[nodiscard]] std::vector<VkFramebuffer> getFramebuffers() const noexcept;

    /** @brief Returns the swapchain object. */
    [[nodiscard]] VkSwapchainKHR getHandle() const noexcept;

    /** @brief Returns true on VK_PRESENT_MODE_MAILBOX being supported on current physical device. */
    [[nodiscard]] bool isMailboxSupported() const noexcept { return m_isMailboxSupported; }

    /** @brief Returns true on VK_PRESENT_MODE_IMMEDIATE being supported on current physical device. */
    [[nodiscard]] bool isImmediateSupported() const noexcept { return m_isImmediateSupported; }

    /** @brief Gets the index to the next image being used in the swapchain.
     *
     *  @param semaphore      Semaphore to signal when the image is usable.
     *  @param fence          Fence to signal when presenting is complete.
     *  @param pImageIndex    The next image index being used in @ref getImages(). 
     *  @param pRecreated     If the swapchain was recreated the render pass might need to recreate some images. (e.g. Depth Buffer)
     */
    bool acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& bRecreated) noexcept;

    /** @brief Destroys and recreates the swapchain, changes present mode.
     *
     *  @param presentMode What present mode to use when swapping, use @ref PhysicalDevice::getPresentModes to get them.
     */
    bool recreate(std::optional<VkPresentModeKHR> presentMode = std::nullopt) noexcept;

private:  

    /** @brief Returns true if the window's VkSurfaceKHR is supported by the VkPhysicalDevice. */
    bool ensureSurfaceSupported() noexcept;

    /** @brief Returns true if an image count was chosen successfully. */
    bool chooseImageCount() noexcept;

    /** @brief Returns true if the surface format was properly found. */
    bool chooseFormat(std::optional<VkSurfaceFormatKHR> surfaceFormat = std::nullopt) noexcept;

    /** @brief Returns true if the present mode was properly found. */
    bool choosePresentMode(std::optional<VkPresentModeKHR> presentMode = std::nullopt) noexcept;

    /** @brief Returns true if an extent was found. */
    bool chooseExtent() noexcept;

    /** @brief Returns true if swapchain images could be obtained. */
    bool obtainImages() noexcept;

    /** @brief Creates image views for frame buffers. */
    bool createImageViews() noexcept;
    bool destroyImageViews() noexcept;
    
    GraphicsPhysicalDevice*         m_pPhysicalDevice;
    GraphicsDevice*                 m_pDevice;
    Window*                         m_pWindow;
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
