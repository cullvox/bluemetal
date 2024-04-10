#include "Core/Print.h"
#include "RenderWindow.h"
#include "PhysicalDevice.h"

namespace bl {

GfxPhysicalDevice::GfxPhysicalDevice(VkPhysicalDevice physicalDevice, uint32_t index)
    : _physicalDevice(physicalDevice)
    , _index(index)
{
}

GfxPhysicalDevice::~GfxPhysicalDevice()
{
}

VkPhysicalDevice GfxPhysicalDevice::get() const
{
    return _physicalDevice; 
}

std::string GfxPhysicalDevice::getVendorName() const
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    switch (properties.vendorID) {
    case 0x1002: return "AMD";
    case 0x1010: return "ImgTec";
    case 0x10DE: return "NVIDIA";
    case 0x13B5: return "ARM";
    case 0x5143: return "Qualcomm";
    case 0x8086: return "INTEL";
    default: return "Undefined Vendor";
    }
}

std::string GfxPhysicalDevice::getDeviceName() const
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    return std::string(properties.deviceName);
}

uint32_t GfxPhysicalDevice::getIndex() const
{ 
    return _index; 
}

VkPhysicalDeviceType GfxPhysicalDevice::getType() const
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    return properties.deviceType;
}

std::vector<VkPresentModeKHR> GfxPhysicalDevice::getPresentModes(std::shared_ptr<RenderWindow> window) const
{
    std::vector<VkPresentModeKHR> presentModes{};
    uint32_t presentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->getSurface(), &presentModeCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan physical device surface present mode count!");
    }

    presentModes.resize(presentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, window->getSurface(), &presentModeCount, presentModes.data())) {
        throw std::runtime_error("Could not get Vulkan physical device surface present modes!");
    }

    return presentModes;
}

std::vector<VkSurfaceFormatKHR> GfxPhysicalDevice::getSurfaceFormats(std::shared_ptr<RenderWindow> window) const
{
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    uint32_t formatCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->getSurface(), &formatCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan physical device surface formats!");
    }

    surfaceFormats.resize(formatCount);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, window->getSurface(), &formatCount, surfaceFormats.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan physical device surface formats!");
    }
    
    return surfaceFormats;
}

VkFormat GfxPhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates) {
        VkFormatProperties properties = {};
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    blError("Could not find any valid format!");
    return VK_FORMAT_UNDEFINED;
}

} // namespace bl