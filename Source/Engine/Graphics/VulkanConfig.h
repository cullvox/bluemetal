#pragma once

#include "Vulkan.h"

namespace bl {

class VulkanConfig {
public:
    static inline constexpr uint32_t apiVersion = VK_API_VERSION_1_3; /** @brief Specifies what version of Vulkan we are using. */
    static inline constexpr uint32_t desiredImageCount = 3; /** @brief Number of images the swapchain will swap through. */
    static inline constexpr uint32_t maxFramesInFlight = 2; /** @brief The number of frames the engine will process at a time. */
    static inline const std::vector validationLayers = { "VK_LAYER_KHRONOS_validation" }; /** @brief Required validation layers. */
};

} // namespace bl