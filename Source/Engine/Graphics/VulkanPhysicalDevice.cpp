#include "Core/Print.h"
#include "Window.h"
#include "VulkanPhysicalDevice.h"

namespace bl {

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice physicalDevice)
    : _physicalDevice(physicalDevice) {
    vkGetPhysicalDeviceProperties(_physicalDevice, &_properties);
}

VkPhysicalDevice VulkanPhysicalDevice::Get() const {
    return _physicalDevice; 
}

std::string VulkanPhysicalDevice::GetVendorName() const {
    switch (_properties.vendorID) {
    case 0x1002: return "AMD";
    case 0x1010: return "ImgTec";
    case 0x10DE: return "NVIDIA";
    case 0x13B5: return "ARM";
    case 0x5143: return "Qualcomm";
    case 0x8086: return "INTEL";
    default: return "Undefined Vendor";
    }
}

std::string VulkanPhysicalDevice::GetDeviceName() const {
    return std::string(_properties.deviceName);
}

VkPhysicalDeviceType VulkanPhysicalDevice::GetType() const {
    return _properties.deviceType;
}

std::vector<VkPresentModeKHR> VulkanPhysicalDevice::GetPresentModes(VulkanWindow* window) const {
    std::vector<VkPresentModeKHR> presentModes{};
    uint32_t presentModeCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, nullptr))
    presentModes.resize(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, presentModes.data()))

    return presentModes;
}

std::vector<VkSurfaceFormatKHR> VulkanPhysicalDevice::GetSurfaceFormats(VulkanWindow* window) const {
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    uint32_t formatCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, nullptr))
    surfaceFormats.resize(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, surfaceFormats.data()))
    
    return surfaceFormats;
}

VkFormat VulkanPhysicalDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
    for (VkFormat format : candidates) {
        VkFormatProperties properties{};
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Could not find a valid format!");
}

const VkPhysicalDeviceProperties& VulkanPhysicalDevice::GetProperties() const {
    return _properties;
}

} // namespace bl