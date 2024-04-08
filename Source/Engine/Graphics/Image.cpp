#include "Image.h"
#include "Core/Print.h"

namespace bl {

GfxImage::GfxImage(std::shared_ptr<GfxDevice> device, const CreateInfo& createInfo)
    : _device(device)
{
    createImage(createInfo);
}

GfxImage::~GfxImage() 
{ 
    vkDestroyImageView(_device->get(), _imageView, nullptr);
    vmaDestroyImage(_device->getAllocator(), _image, _allocation);
}

void GfxImage::createImage(const CreateInfo& createInfo)
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
    imageCreateInfo.extent = (VkExtent3D)_extent;
    imageCreateInfo.mipLevels = createInfo.mipLevels;
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

    if (vmaCreateImage(_device->getAllocator(), &imageCreateInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Vulkan image!");
    }

    VkComponentMapping componentMapping = {};
    componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = createInfo.aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = createInfo.mipLevels;
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

    if (vkCreateImageView(_device->get(), &viewCreateInfo, nullptr, &_imageView) != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Vulkan image view!");
    }
}

VkExtent3D GfxImage::getExtent() const { return _extent; }
VkFormat GfxImage::getFormat() const { return _format; }
VkImageUsageFlags GfxImage::getUsage() const { return _usage; }
VkImageView GfxImage::getDefaultView() const { return _imageView; }
VkImage GfxImage::get() const { return _image; }

} // namespace bl
