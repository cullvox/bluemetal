#include "Core/Print.h"
#include "VulkanWindow.h"
#include "VulkanPhysicalDevice.h"

namespace bl {

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice physicalDevice)
    : _physicalDevice(physicalDevice) 
{
    vkGetPhysicalDeviceProperties(_physicalDevice, &_properties);
}

VkPhysicalDevice VulkanPhysicalDevice::Get() const 
{
    return _physicalDevice; 
}

std::string VulkanPhysicalDevice::GetVendorName() const 
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

std::string VulkanPhysicalDevice::GetDeviceName() const 
{
    return std::string(_properties.deviceName);
}

VkPhysicalDeviceType VulkanPhysicalDevice::GetType() const 
{
    return _properties.deviceType;
}

const std::vector<VkPresentModeKHR>& VulkanPhysicalDevice::GetPresentModes(VulkanWindow* window) 
{
    if (!_presentModes.empty())
        return _presentModes;

    uint32_t presentModeCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, nullptr))
    _presentModes.resize(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->GetSurface(), &presentModeCount, _presentModes.data()))

    return _presentModes;
}

const std::vector<VkSurfaceFormatKHR>& VulkanPhysicalDevice::GetSurfaceFormats(VulkanWindow* window) 
{
    if (!_surfaceFormats.empty())
        return _surfaceFormats;

    uint32_t formatCount = 0;

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, nullptr))
    _surfaceFormats.resize(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->GetSurface(), &formatCount, _surfaceFormats.data()))

    return _surfaceFormats;
}

std::optional<VkImageFormatProperties2> VulkanPhysicalDevice::GetImageFormatProperties(const VkPhysicalDeviceImageFormatInfo2& info)
{
    VkImageFormatProperties2 properties{};
    properties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
    properties.pNext = nullptr;

    VkResult result = vkGetPhysicalDeviceImageFormatProperties2(_physicalDevice, &info, &properties);

    if (result == VK_ERROR_FORMAT_NOT_SUPPORTED)
        return std::nullopt;
    else if (result == VK_SUCCESS)
        return properties;
    else 
        VK_CHECK(result)
}

VkFormat VulkanPhysicalDevice::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const 
{
    for (VkFormat format : candidates) 
    {
        VkFormatProperties properties{};
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) 
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) 
        {
            return format;
        }
    }

    throw std::runtime_error("Could not find a valid format!");
}

const VkPhysicalDeviceProperties& VulkanPhysicalDevice::GetProperties() const 
{
    return _properties;
}

} // namespace bl