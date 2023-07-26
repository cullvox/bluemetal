#include "PhysicalDevice.h"

namespace bl
{

PhysicalDevice::PhysicalDevice(uint32_t index, VkPhysicalDevice physicalDevice)
    : m_index(index)
    , m_physicalDevice(physicalDevice)
{
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

VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties{};
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