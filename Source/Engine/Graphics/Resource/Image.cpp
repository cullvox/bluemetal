#include "Image.h"
#include "Core/Print.h"

namespace bl 
{

Image::Image(Device* device, VkImageType type, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, uint32_t mipLevels)
    : _device(device)
    , _type(type)
    , _extent(extent)
    , _format(format)
    , _usage(usage)
    , _aspectMask(aspectMask)
    , _mipLevels(mipLevels)
{
    CreateImage(createInfo);
}

Image::Image(Image& rhs)
    : _device(rhs.device)
    , _type(rhs.type)
    , _extent(rhs.extent)
    , _format(rhs.format)
    , _usage(rhs.usage)
    , _aspectMask(rhs.aspectMask)
    , _mipLevels(rhs.mipLevels)
{
    CreateImage();

}

Image::Image(Image&& move)
{

}

Image::~Image() 
{ 
    vkDestroyImageView(_device->get(), _imageView, nullptr);
    vmaDestroyImage(_device->getAllocator(), _image, _allocation);
}

VkExtent3D Image::GetExtent() const 
{ 
    return _extent; 
}

VkFormat Image::GetFormat() const 
{ 
    return _format;
}

VkImageUsageFlags Image::GetUsage() const 
{ 
    return _usage;
}

VkImageView Image::GetView() const 
{
    return _imageView;
}

VkImage Image::Get() const 
{ 
    return _image;
}

void Image::CreateImage(const CreateInfo& createInfo)
{
    _type = createInfo.type;
    _format = createInfo.format;
    _extent = createInfo.extent;
    _usage = createInfo.usage;

    auto graphicsFamilyIndex = _device->getGraphicsFamilyIndex();

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = {};
    imageCreateInfo.imageType = _type;
    imageCreateInfo.format = _format;
    imageCreateInfo.extent = _extent;
    imageCreateInfo.mipLevels = _mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = _usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 1;
    imageCreateInfo.pQueueFamilyIndices = &graphicsFamilyIndex;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = 0;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 1.0f;

    VK_CHECK(vmaCreateImage(_device->getAllocator(), &imageCreateInfo, &allocationCreateInfo, &_image, &_allocation, nullptr))

    VkComponentMapping componentMapping = {};
    componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = _aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = _mipLevels;
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

    VK_CHECK(vkCreateImageView(_device->get(), &viewCreateInfo, nullptr, &_imageView))
}

void Image::Copy(Image& other)
{
    _device->ImmediateSubmit([](VkCommandBuffer cmd){
        vmaCopy
    });
}

} // namespace bl
