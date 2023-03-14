#include "Core/Log.hpp"
#include "Render/Image.hpp"

namespace bl {

Image::Image() noexcept
    : m_pRenderDevice(nullptr)
    , m_image(VK_NULL_HANDLE)
    , m_allocation(VK_NULL_HANDLE)
    , m_imageView(VK_NULL_HANDLE)
{
}

Image::Image(RenderDevice& renderDevice, VkImageType type, VkFormat format, Extent2D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask) noexcept
    : Image()
{
    m_pRenderDevice = &renderDevice;

    const uint32_t graphicsFamilyIndex = m_pRenderDevice->getGraphicsFamilyIndex();
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
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
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

    if (vmaCreateImage(m_pRenderDevice->getAllocator(), &createInfo, &allocationCreateInfo, &m_image, &m_allocation, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not create an image!\n");
        collapse();
        return;
    }

    const VkImageViewCreateInfo viewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = m_image,
        .viewType = (VkImageViewType)type,
        .format = format,
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY, .g = VK_COMPONENT_SWIZZLE_IDENTITY, .b = VK_COMPONENT_SWIZZLE_IDENTITY, .a = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = aspectMask, .baseMipLevel = 0, .levelCount = mipLevels, .baseArrayLayer = 0, .layerCount = 1} 
    };

    if (vkCreateImageView(m_pRenderDevice->getDevice(), &viewCreateInfo, nullptr, &m_imageView) != VK_SUCCESS)
    {
        Logger::Error("Could not create a vulkan image's view!\n");
        collapse();
        return;
    }
}

Image::~Image()
{
    collapse();
}

Image& Image::operator=(Image&& rhs) noexcept
{
    // In order to assign another image we must actually destroy the first one.
    collapse();

    m_pRenderDevice = std::move(rhs.m_pRenderDevice);
    m_image = std::move(rhs.m_image);
    m_allocation = std::move(rhs.m_allocation);
    m_imageView = std::move(rhs.m_imageView);

    rhs.m_pRenderDevice = nullptr;
    rhs.m_image = VK_NULL_HANDLE;
    rhs.m_allocation = VK_NULL_HANDLE;
    rhs.m_imageView = VK_NULL_HANDLE;

    rhs.collapse();

    return *this;
}

VkImage Image::getImage() const noexcept
{
    return m_image;
}

VkImageView Image::getImageView() const noexcept
{
    return m_imageView;
}

void Image::collapse() noexcept
{
    if (m_imageView)vkDestroyImageView(m_pRenderDevice->getDevice(), m_imageView, nullptr);
    if (m_image) vmaDestroyImage(m_pRenderDevice->getAllocator(), m_image, m_allocation);
    
    m_imageView = VK_NULL_HANDLE;
    m_image = VK_NULL_HANDLE;
}

} // namespace bl