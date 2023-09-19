#include "PhysicalDevice.h"
#include "Window/Window.h"

namespace bl {

GraphicsPhysicalDevice::GraphicsPhysicalDevice(VkPhysicalDevice physicalDevice, uint32_t index)
    : m_physicalDevice(physicalDevice)
    , m_index(index)
{
}

VkPhysicalDevice GraphicsPhysicalDevice::getHandle() { return m_physicalDevice; }

std::string GraphicsPhysicalDevice::getVendorName()
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    switch (properties.vendorID) {
    case 0x1002:
        return "AMD";
    case 0x1010:
        return "ImgTec";
    case 0x10DE:
        return "NVIDIA";
    case 0x13B5:
        return "ARM";
    case 0x5143:
        return "Qualcomm";
    case 0x8086:
        return "INTEL";
    default:
        return "Undefined Vendor";
    }
}

std::string GraphicsPhysicalDevice::getDeviceName()
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    return std::string(properties.deviceName);
}

uint32_t GraphicsPhysicalDevice::getIndex() { return m_index; }

std::vector<VkPresentModeKHR> GraphicsPhysicalDevice::getPresentModes(Window* pWindow)
{
    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes = {};
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_physicalDevice, pWindow->getSurface(), &presentModeCount, nullptr)
        != VK_SUCCESS) {
        throw std::runtime_error(
            "Could not get Vulkan physical device surface present mode count!");
    }

    presentModes.resize(presentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_physicalDevice, pWindow->getSurface(), &presentModeCount, presentModes.data())) {
        throw std::runtime_error("Could not get Vulkan physical device surface present modes!");
    }

    return presentModes;
}

std::vector<VkSurfaceFormatKHR> GraphicsPhysicalDevice::getSurfaceFormats(Window* pWindow)
{
    uint32_t formatCount = 0;
    std::vector<VkSurfaceFormatKHR> formats = {};
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_physicalDevice, pWindow->getSurface(), &formatCount, nullptr)
        != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan physical device surface formats!");
    }

    formats.resize(formatCount);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_physicalDevice, pWindow->getSurface(), &formatCount, formats.data())
        != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan physical device surface formats!");
    }

    return formats;
}

VkFormat GraphicsPhysicalDevice::findSupportedFormat(
    const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties properties = {};
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR
            && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL
            && (properties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Could not find any valid format!");
    return VK_FORMAT_UNDEFINED;
}

} // namespace bl