#include "Core/Log.hpp"
#include "Render/Image.hpp"

blImage::blImage(std::shared_ptr<blRenderDevice> renderDevice, VkImageType type, VkFormat format, blExtent2D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask)
    : _renderDevice(renderDevice)
    , _extent(extent)
    , _type(type)
    , _format(format)
    , _usage(usage)
{
    const uint32_t graphicsFamilyIndex = _renderDevice->getGraphicsFamilyIndex();
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

    if (vmaCreateImage(_renderDevice->getAllocator(), &createInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan image!");
    }


    const VkComponentMapping componentMapping{
        .r = VK_COMPONENT_SWIZZLE_IDENTITY, 
        .g = VK_COMPONENT_SWIZZLE_IDENTITY, 
        .b = VK_COMPONENT_SWIZZLE_IDENTITY, 
        .a = VK_COMPONENT_SWIZZLE_IDENTITY
    };
    const VkImageSubresourceRange subresourceRange{
        .aspectMask = aspectMask, 
        .baseMipLevel = 0, 
        .levelCount = mipLevels, 
        .baseArrayLayer = 0, 
        .layerCount = 1 
    };
    const VkImageViewCreateInfo viewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = _image,
        .viewType = (VkImageViewType)_type,
        .format = _format,
        .components = componentMapping,
        .subresourceRange = subresourceRange,
    };

    if (vkCreateImageView(_renderDevice->getDevice(), &viewCreateInfo, nullptr, &_imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan image's view!");
    }
}

blImage::~blImage()
{
    collapse();
}

blExtent2D blImage::getExtent() const noexcept
{
    return _extent;
}

VkFormat blImage::getFormat() const noexcept
{
    return _format;
}

VkImageUsageFlags blImage::getUsage() const noexcept
{
    return _usage;
}

VkImageView blImage::getImageView() const noexcept
{
    return _imageView;
}

VkImage blImage::getImage() const noexcept
{
    return _image;
}

void blImage::collapse() noexcept
{
    if (_imageView) vkDestroyImageView(_renderDevice->getDevice(), _imageView, nullptr);
    if (_image) vmaDestroyImage(_renderDevice->getAllocator(), _image, _allocation);
    
    _renderDevice = nullptr;
    _extent = {0, 0};
    _type = VK_IMAGE_TYPE_1D;
    _format = VK_FORMAT_UNDEFINED;
    _usage = 0;
    _imageView = VK_NULL_HANDLE;
    _image = VK_NULL_HANDLE;
}