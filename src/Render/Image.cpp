#include "Render/Image.hpp"
#include <vulkan/vulkan_core.h>

namespace bl {

Image::Image(RenderDevice& renderDevice, VkImageType type, VkFormat format, Extent2D extent, uint32_t mipLevels, VkImageUsageFlags usage)
    : m_renderDevice(renderDevice)
{
    const uint32_t graphicsFamilyIndex = m_renderDevice.getGraphicsFamilyIndex();
    const VkImageCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = type,
        .format = format,
        .extent = VkExtent3D{(uint32_t)extent.width, (uint32_t)extent.height, 1},
        .mipLevels = mipLevels,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphicsFamilyIndex,
        .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    const VmaAllocationCreateInfo allocationCreateInfo{
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 1.0f
    };

    if (vmaCreateImage(m_renderDevice.getAllocator(), &createInfo, &allocationCreateInfo, &m_image, &m_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create an image!");
    }
}

Image::~Image()
{
    vmaDestroyImage(m_renderDevice.getAllocator(), m_image, m_allocation);
}

} // namespace bl