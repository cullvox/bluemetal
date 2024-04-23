#pragma once

#include "Precompiled.h"

namespace bl
{

class GraphicsConfig
{
public:
    static inline constexpr uint32_t apiVersion = VK_API_VERSION_1_3; /** @brief Specifies what version of Vulkan we are using. */
    static inline constexpr uint32_t desiredImageCount = 3; /** @brief Number of images the swapchain will swap through. */
    static inline constexpr VkFormat defaultPresentFormat = VK_FORMAT_R8G8B8A8_SRGB; /** @brief Default format to look for. */
    static inline constexpr VkColorSpaceKHR defaultPresentColorspace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; /** @brief Default color space to look for. */
    static inline constexpr VkSurfaceFormatKHR defaultSurfaceFormat = VkSurfaceFormatKHR{defaultPresentFormat, defaultPresentColorspace};
    static inline constexpr VkPresentModeKHR defaultPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; /** @brief Desired present mode to use in swapchain. Will use present mode FIFO if this is unavailable.*/
    static inline constexpr uint32_t numFramesInFlight = 2; /** @brief The number of frames the engine will process at a time. */
};

} // namespace bl