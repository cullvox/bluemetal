#include "Image.h"
#include "Core/Log.h"

namespace bl
{

Image::Image(
    GraphicsDevice*     pDevice, 
    VkImageType         type, 
    VkFormat            format, 
    VkExtent3D          extent,  
    VkImageUsageFlags   usage, 
    VkImageAspectFlags  aspectMask,
    uint32_t            mipLevels)
    : m_pDevice(pDevice)
    , m_extent(extent)
    , m_type(type)
    , m_format(format)
    , m_usage(usage)
{
    auto graphicsFamilyIndex = m_pDevice->getGraphicsFamilyIndex();

    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = {};
    createInfo.imageType = type;
    createInfo.format = format;
    createInfo.extent = extent;
    createInfo.mipLevels = mipLevels;
    createInfo.arrayLayers = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &graphicsFamilyIndex;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = 0;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 1.0f;

    if (vmaCreateImage(m_pDevice->getAllocator(), &createInfo, &allocationCreateInfo, &m_image, &m_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan image!");
    }

    VkComponentMapping componentMapping = {};
    componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
    componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY; 
    componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY; 
    componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY; 

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMask; 
    subresourceRange.baseMipLevel = 0; 
    subresourceRange.levelCount = mipLevels; 
    subresourceRange.baseArrayLayer = 0; 
    subresourceRange.layerCount = 1;

    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.pNext = nullptr;
    viewCreateInfo.flags = 0;
    viewCreateInfo.image = m_image;
    viewCreateInfo.viewType = (VkImageViewType)m_type;
    viewCreateInfo.format = m_format;
    viewCreateInfo.components = componentMapping;
    viewCreateInfo.subresourceRange = subresourceRange;

    if (vkCreateImageView(m_pDevice->getHandle(), &viewCreateInfo, nullptr, &m_imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan image view!");
    }

}

Image::~Image()
{
    vmaDestroyImage(m_pDevice->getAllocator(), m_image, m_allocation);
}

Extent3D Image::getExtent()
{
    return m_extent;
}

VkFormat Image::getFormat()
{
    return m_format;
}

VkImageUsageFlags Image::getUsage()
{
    return m_usage;
}

VkImageView Image::getDefaultView()
{
    return m_imageView;
}

VkImage Image::getHandle()
{
    return m_image;
}

} // namespace bl