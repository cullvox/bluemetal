#pragma once

#include "WindowGLFW.h"

namespace bl {

class VulkanPlatformGLFW {
    static std::vector<const char*> GetInstanceExtensions();
    static VkSurfaceKHR CreateSurface(WindowInterface* window);
};

} // namespace bl