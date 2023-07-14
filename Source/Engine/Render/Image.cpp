#include "Core/Log.hpp"
#include "Render/Image.hpp"
#include "Math/Vector2.hpp"

blImage::blImage(std::shared_ptr<blRenderDevice> renderDevice, vk::ImageType type, vk::Format format, blExtent3D extent, uint32_t mipLevels, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask)
    : _renderDevice(renderDevice)
    , _extent(extent)
    , _type(type)
    , _format(format)
    , _usage(usage)
{
    uint32_t graphicsFamilyIndex = _renderDevice->getGraphicsFamilyIndex();

    const vk::ImageCreateInfo createInfo
    {
        {},                             // flags
        _type,                          // imageType
        _format,                        // format
        (vk::Extent3D)extent,           // extent
        mipLevels,                      // mipLevels
        1,                              // arrayLayers
        vk::SampleCountFlagBits::e1,    // samples
        vk::ImageTiling::eOptimal,      // tiling
        _usage,                         // usage
        vk::SharingMode::eExclusive,    // sharingMode
        1,                              // queueFamilyIndexCount
        &graphicsFamilyIndex,           // pQueueFamilyIndices
        vk::ImageLayout::eUndefined,    // initialLayout
    };

    const VmaAllocationCreateInfo allocationCreateInfo
    {
        0,                                      // flags
        VMA_MEMORY_USAGE_GPU_ONLY,              // usage
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,    // requiredFlags
        0,                                      // preferredFlags
        0,                                      // memoryTypeBits
        VK_NULL_HANDLE,                         // pool
        nullptr,                                // pUserData
        1.0f,                                   // priority
    };

    if (vmaCreateImage(_renderDevice.getAllocator(), (VkImageCreateInfo*)&createInfo, &allocationCreateInfo, (VkImage*)&_image, &_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan image!");
    }

    const vk::ComponentMapping componentMapping
    {
        vk::ComponentSwizzle::eIdentity, // r  
        vk::ComponentSwizzle::eIdentity, // g  
        vk::ComponentSwizzle::eIdentity, // b 
        vk::ComponentSwizzle::eIdentity  // a 
    };

    const vk::ImageSubresourceRange subresourceRange
    {
        aspectMask, // aspectMask 
        0,          // baseMipLevel 
        mipLevels,  // levelCount 
        0,          // baseArrayLayer 
        1,          // layerCount
    };

    const vk::ImageViewType viewType = (vk::ImageViewType)_type;
    const vk::ImageViewCreateInfo viewCreateInfo
    {
        {},                 // flags
        _image,             // image
        viewType,           // viewType
        _format,            // format
        componentMapping,   // components
        subresourceRange,   // subresourceRange
    };

    _imageView = 
        _renderDevice
            .getDevice()
            .createImageViewUnique(viewCreateInfo);
}

blImage::~blImage()
{
    if (_image) vmaDestroyImage(_renderDevice.getAllocator(), _image, _allocation);
}

blExtent2D blImage::getExtent() const noexcept
{
    return _extent;
}

vk::Format blImage::getFormat() const noexcept
{
    return _format;
}

vk::ImageUsageFlags blImage::getUsage() const noexcept
{
    return _usage;
}

vk::ImageView blImage::getImageView() const noexcept
{
    return _imageView.get();
}

vk::Image blImage::getImage() const noexcept
{
    return _image;
}