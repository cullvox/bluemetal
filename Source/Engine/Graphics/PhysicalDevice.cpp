#include "Core/Print.h"
#include "RenderWindow.h"
#include "PhysicalDevice.h"

namespace bl 
{

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice)
    : _physicalDevice(physicalDevice)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice, &_properties);
}

PhysicalDevice::~PhysicalDevice()
{
}

VkPhysicalDevice PhysicalDevice::Get() const
{
    return _physicalDevice; 
}

std::string PhysicalDevice::GetVendorName() const
{
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

std::string PhysicalDevice::GetDeviceName() const
{
    return std::string(_properties.deviceName);
}

VkPhysicalDeviceType PhysicalDevice::GetType() const
{
    return _properties.deviceType;
}

std::vector<VkPresentModeKHR> PhysicalDevice::GetPresentModes(Window& window) const
{
    std::vector<VkPresentModeKHR> presentModes{};
    uint32_t presentModeCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window.GetSurface(), &presentModeCount, nullptr))
    presentModes.resize(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window.GetSurface(), &presentModeCount, presentModes.data()))

    return presentModes;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::getSurfaceFormats(Window& window) const
{
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    uint32_t formatCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->getSurface(), &formatCount, nullptr))
    surfaceFormats.resize(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->getSurface(), &formatCount, surfaceFormats.data()))
    
    return surfaceFormats;
}

VkFormat PhysicalDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
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

} // namespace bl