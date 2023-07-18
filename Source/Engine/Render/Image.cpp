#include "Image.h"
#include "Core/Log.h"

blImage::blImage(std::shared_ptr<blDevice> device, VkImageType type, VkFormat format, blExtent3D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask)
    : _device(device)
    , _extent(extent)
    , _type(type)
    , _format(format)
    , _usage(usage)
{
    uint32_t graphicsFamilyIndex = _device->getGraphicsFamilyIndex();

    const VkImageCreateInfo createInfo
    {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,    // sType
        nullptr,                                // pNext
        {},                                     // flags
        _type,                                  // imageType
        _format,                                // format
        (VkExtent3D)extent,                     // extent
        mipLevels,                              // mipLevels
        1,                                      // arrayLayers
        VK_SAMPLE_COUNT_1_BIT,                  // samples
        VK_IMAGE_TILING_OPTIMAL,                // tiling
        _usage,                                 // usage
        VK_SHARING_MODE_EXCLUSIVE,              // sharingMode
        1,                                      // queueFamilyIndexCount
        &graphicsFamilyIndex,                   // pQueueFamilyIndices
        VK_IMAGE_LAYOUT_UNDEFINED,              // initialLayout
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

    if (vmaCreateImage(_device->getAllocator(), &createInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan image!");
    }

    const VkComponentMapping componentMapping
    {
        VK_COMPONENT_SWIZZLE_IDENTITY, // r  
        VK_COMPONENT_SWIZZLE_IDENTITY, // g  
        VK_COMPONENT_SWIZZLE_IDENTITY, // b 
        VK_COMPONENT_SWIZZLE_IDENTITY, // a 
    };

    const VkImageSubresourceRange subresourceRange
    {
        aspectMask, // aspectMask 
        0,          // baseMipLevel 
        mipLevels,  // levelCount 
        0,          // baseArrayLayer 
        1,          // layerCount
    };

    const VkImageViewCreateInfo viewCreateInfo
    {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // sType
        nullptr,                                    // pNext
        0,                                          // flags
        _image,                                     // image
        (VkImageViewType)_type,                     // viewType
        _format,                                    // format
        componentMapping,                           // components
        subresourceRange,                           // subresourceRange
    };

    if (vkCreateImageView(_device->getDevice(), &viewCreateInfo, nullptr, &_imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan image view!");
    }

}

blImage::~blImage()
{
    vmaDestroyImage(_device->getAllocator(), _image, _allocation);
}

blExtent3D blImage::getExtent() const noexcept
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