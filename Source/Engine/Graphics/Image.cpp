#include "Image.h"
#include "Core/Print.h"

namespace bl 
{

Image::Image(Device* device, VkImageType type, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkImageLayout initialLayout, uint32_t mipLevels)
    : _device(device)
    , _extent(extent)
    , _type(type)
    , _format(format)
    , _usage(usage)
    , _aspectMask(aspectMask)
    , _layout(initialLayout)
    , _mipLevels(mipLevels) {
    CreateImage();
}

Image::Image(Image&& rhs)
    : _device(rhs._device)
    , _extent(rhs._extent)
    , _type(rhs._type)
    , _format(rhs._format)
    , _usage(rhs._usage)
    , _aspectMask(rhs._aspectMask)
    , _mipLevels(rhs._mipLevels)
    , _image(rhs._image)
    , _imageView(rhs._imageView)
    , _allocation(rhs._allocation) {
    rhs._device = {};
    rhs._type = {};
    rhs._extent = {};
    rhs._format = {};
    rhs._usage = {};
    rhs._aspectMask = {};
    rhs._mipLevels = {};
    rhs._image = {};
    rhs._imageView = {};
    rhs._allocation = {};
}

Image::~Image() { 
    vkDestroyImageView(_device->Get(), _imageView, nullptr);
    vmaDestroyImage(_device->GetAllocator(), _image, _allocation);
}

VkExtent3D Image::GetExtent() const { 
    return _extent; 
}

VkFormat Image::GetFormat() const { 
    return _format;
}

VkImageUsageFlags Image::GetUsage() const {
    return _usage;
}

VkImageLayout Image::GetLayout() const {
    return _layout;
}

VkImage Image::Get() const {
    return _image;
}

VkImageView Image::GetView() const {
    return _imageView;
}

void Image::Transition(VkImageLayout layout) {

    _device->ImmediateSubmit([&](VkCommandBuffer cmd){
        
        VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
        VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        
        if (_layout == VK_IMAGE_LAYOUT_UNDEFINED && layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::runtime_error("Unsupported image layout transition!");
        }
        
        barrier.oldLayout = _layout;
        barrier.newLayout = layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            cmd,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    });

    _layout = layout;
}

void Image::CreateImage() {
    auto graphicsFamilyIndex = _device->GetGraphicsFamilyIndex();

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
    imageCreateInfo.initialLayout = _layout;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = 0;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 1.0f;

    VK_CHECK(vmaCreateImage(_device->GetAllocator(), &imageCreateInfo, &allocationCreateInfo, &_image, &_allocation, nullptr))

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

    VK_CHECK(vkCreateImageView(_device->Get(), &viewCreateInfo, nullptr, &_imageView))
}

} // namespace bl
