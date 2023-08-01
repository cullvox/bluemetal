#include "PhysicalDevice.h"
#include "Window/Window.h"

namespace bl
{

PhysicalDevice::PhysicalDevice(uint32_t index, VkPhysicalDevice physicalDevice)
    : m_index(index)
    , m_physicalDevice(physicalDevice)
{
}

PhysicalDevice::~PhysicalDevice()
{
}

VkPhysicalDevice PhysicalDevice::getHandle()
{
    return m_physicalDevice;
}

std::string PhysicalDevice::getVendorName()
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    switch (properties.vendorID)
    {
        case 0x1002: return "AMD";
        case 0x1010: return "ImgTec";
        case 0x10DE: return "NVIDIA";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x8086: return "INTEL";
        default: return "Undefined Vendor";
    }
}

std::string PhysicalDevice::getDeviceName()
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    return std::string(properties.deviceName);
}

uint32_t PhysicalDevice::getIndex()
{
    return m_index;
}

std::vector<VkPresentModeKHR> PhysicalDevice::getPresentModes(std::shared_ptr<Window> window)
{
    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes = {};
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, window->getSurface(), &presentModeCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface present mode count!");
    }

    presentModes.resize(presentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, window->getSurface(), &presentModeCount, presentModes.data()))
    {
        throw std::runtime_error("Could not get Vulkan physical device surface present modes!");
    }

    return presentModes;
}

std::vector<VkSurfaceFormatKHR> PhysicalDevice::getSurfaceFormats(std::shared_ptr<Window> window)
{
    uint32_t formatCount = 0;
    std::vector<VkSurfaceFormatKHR> formats = {};
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, window->getSurface(), &formatCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface formats!");
    }

    formats.resize(formatCount);
    
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, window->getSurface(), &formatCount, formats.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface formats!");
    }

    return formats;
}

VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties = {};
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Could not find any valid format!");
    return VK_FORMAT_UNDEFINED;
}

} // namespace bl