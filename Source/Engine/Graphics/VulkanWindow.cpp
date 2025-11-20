#include "VulkanWindow.h"

namespace bl {

VulkanWindow::VulkanWindow(VulkanDevice* device, const std::string& title, Rect2D rect, bool fullscreen)
    : Window(title, rect, fullscreen)
    , _device(device)
{
    if (!SDL_Vulkan_CreateSurface(Get(), _device->GetInstance()->Get(), nullptr, &_surface)) {
        throw std::runtime_error("Could not create a vulkan window surface!");
    }

    _swapchain = std::make_unique<VulkanSwapchain>(_device, this);
}

VulkanWindow::~VulkanWindow()
{
    _swapchain.reset();
    vkDestroySurfaceKHR(_device->GetInstance()->Get(), _surface, nullptr);
}

VulkanDevice* VulkanWindow::GetDevice() const
{
    return _device;
}

VkSurfaceKHR VulkanWindow::GetSurface()
{
    return _surface;
}

VulkanSwapchain* VulkanWindow::GetSwapchain()
{
    return _swapchain.get();
}

std::span<const char*> VulkanWindow::GetVulkanExtensions()
{
    static uint32_t extensionCount = 0;
    static const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    static std::vector<const char*> ext { extensions, extensions + extensionCount };

    return std::span<const char*> { ext.begin(), ext.end() };
}

// std::vector<VkPresentModeKHR> VulkanWindow::GetPresentModes(VulkanWindow* window) const {
//     std::vector<VkPresentModeKHR> presentModes{};
//     uint32_t presentModeCount = 0;
//
//     VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, nullptr))
//     presentModes.resize(presentModeCount);
//     VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, presentModes.data()))
//
//     return presentModes;
// }

// std::vector<VkSurfaceFormatKHR> VulkanWindow::GetSurfaceFormats(VulkanWindow* window) const {
//     std::vector<VkSurfaceFormatKHR> surfaceFormats{};
//     uint32_t formatCount = 0;
//
//     VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, nullptr))
//     surfaceFormats.resize(formatCount);
//     VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, surfaceFormats.data()))
//
//     return surfaceFormats;
// }

}