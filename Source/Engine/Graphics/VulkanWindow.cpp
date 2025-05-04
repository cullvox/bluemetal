#include "VulkanWindow.h"

std::vector<VkPresentModeKHR> VulkanWindow::GetPresentModes(VulkanWindow* window) const {
    std::vector<VkPresentModeKHR> presentModes{};
    uint32_t presentModeCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, nullptr))
    presentModes.resize(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, presentModes.data()))

    return presentModes;
}

std::vector<VkSurfaceFormatKHR> VulkanWindow::GetSurfaceFormats(VulkanWindow* window) const {
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    uint32_t formatCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, nullptr))
    surfaceFormats.resize(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, surfaceFormats.data()))
    
    return surfaceFormats;
}