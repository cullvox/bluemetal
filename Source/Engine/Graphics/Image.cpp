#include "Image.h"
#include "Core/Print.h"

namespace bl {

Image::Image() { }

Image::Image(const ImageCreateInfo& info)
{
    assert(create(info) && "Could not create an image, check prevous error logs.");
}

Image::~Image() { destroy(); }

Image& Image::operator=(Image&& rhs) noexcept
{
    destroy();
    
    _pDevice = rhs._pDevice;
    _extent = rhs._extent;
    _type = rhs._type;
    _format = rhs._format;
    _usage = rhs._usage;
    _image = rhs._image;
    _imageView = rhs._imageView;
    _allocation = rhs._allocation;

    rhs._pDevice = {};
    rhs._extent = {};
    rhs._type = {};
    rhs._format = {};
    rhs._usage = {};
    rhs._image = {};
    rhs._imageView = {};
    rhs._allocation = {};

    return *this;
}

bool Image::create(const ImageCreateInfo& createInfo) noexcept
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

    if (vmaCreateImage(_pDevice->getAllocator(), &createInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) != VK_SUCCESS) {
        blError("Could not create a Vulkan image!");
        return false;
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
    viewCreateInfo.image = _image;
    viewCreateInfo.viewType = (VkImageViewType)_type;
    viewCreateInfo.format = _format;
    viewCreateInfo.components = componentMapping;
    viewCreateInfo.subresourceRange = subresourceRange;

    if (vkCreateImageView(_pDevice->getHandle(), &viewCreateInfo, nullptr, &_imageView) != VK_SUCCESS) {
        blInfo("Could not create a Vulkan image view!");
        return false;
    }

    return true;
}

void Image::destroy() noexcept
{
    if (_image == VK_NULL_HANDLE) return;
    vmaDestroyImage(_pDevice->getAllocator(), _image, _allocation);
}

Extent3D Image::getExtent() const noexcept { return _extent; }
VkFormat Image::getFormat() const noexcept { return _format; }
VkImageUsageFlags Image::getUsage() const noexcept { return _usage; }
VkImageView Image::getDefaultView() const noexcept { return _imageView; }
VkImage Image::getHandle() const noexcept { return _image; }

} // namespace bl
