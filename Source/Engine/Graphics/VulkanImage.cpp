#include "VulkanImage.h"
#include "VulkanPhysicalDevice.h"
#include "Precompiled.h"
#include "VulkanBuffer.h"
#include <vulkan/vulkan_core.h>

namespace bl {

VulkanImage::VulkanImage()
    : _device(nullptr)
    , _extent({})
    , _type(VK_IMAGE_TYPE_1D)
    , _format(VK_FORMAT_UNDEFINED)
    , _usage(0)
    , _mipLevels(1)
    , _layout(VK_IMAGE_LAYOUT_UNDEFINED)
    , _defaultView(VK_NULL_HANDLE)
{
}

VulkanImage::VulkanImage(
    VulkanDevice*           device, 
    VkImageType             type, 
    VkExtent3D              extent, 
    VkFormat                format, 
    VkImageUsageFlags       usage, 
    bool                    generateMipmaps, 
    VkSampleCountFlagBits   samples, 
    VkImageLayout           initialLayout)
    : _device(device)
    , _extent(extent)
    , _type(type)
    , _format(format)
    , _usage(usage)
    , _mipLevels(1)
    , _samples(samples)
    , _layout(initialLayout)
    , _defaultView(VK_NULL_HANDLE)
    , _owned(true)
{
    auto physicalDevice = _device->GetPhysicalDevice();

    // Ensure that the format is supported.
    VkPhysicalDeviceImageFormatInfo2 imageFormatInfo {};
    imageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
    imageFormatInfo.pNext = nullptr;
    imageFormatInfo.flags = 0;
    imageFormatInfo.format = format;
    imageFormatInfo.usage = usage;
    imageFormatInfo.type = type;
    imageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    auto imageFormatProperties = physicalDevice->GetImageFormatProperties(imageFormatInfo);
    if (!imageFormatProperties.has_value()) {
        throw std::runtime_error("Image format is not supported!");
    }

    auto graphicsFamilyIndex = _device->GetGraphicsFamilyIndex();

    if (generateMipmaps) {
        _mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_extent.width, _extent.height)))) + 1;
        _usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    }

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = {};
    imageCreateInfo.imageType = _type;
    imageCreateInfo.format = _format;
    imageCreateInfo.extent = _extent;
    imageCreateInfo.mipLevels = _mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = samples;
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

    _defaultView = CreateView(_usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT, _mipLevels);
}

VulkanImage::VulkanImage(
    VulkanDevice*           device, 
    VkImage                 image, 
    VkImageType             type, 
    VkExtent3D              extent, 
    VkFormat                format,
    VkImageUsageFlags       usage, 
    VkSampleCountFlagBits   samples,
    VkImageLayout           layout)
    : _device(device)
    , _extent(extent)
    , _type(type)
    , _format(format)
    , _usage(usage)
    , _mipLevels(1)
    , _samples(samples)
    , _layout(layout)
    , _image(image)
    , _defaultView(VK_NULL_HANDLE)
    , _owned(false)
{
}

VulkanImage::VulkanImage(VulkanImage&& rhs)
{
    this->operator=(std::move(rhs));
}

VulkanImage::~VulkanImage()
{
    if (!_device || !_owned)
        return;

    for (auto view : _views) {
        vkDestroyImageView(_device->Get(), view, nullptr);
    }

    if (_image)
        vmaDestroyImage(_device->GetAllocator(), _image, _allocation);
}

VulkanImage& VulkanImage::operator=(VulkanImage&& rhs)
{
    _device = rhs._device;
    _extent = rhs._extent;
    _type = rhs._type;
    _format = rhs._format;
    _usage = rhs._usage;
    _image = rhs._image;
    _views = rhs._views;
    _allocation = rhs._allocation;
    _defaultView = rhs._defaultView;
    _owned = rhs._owned;

    rhs._device = {};
    rhs._type = {};
    rhs._extent = {};
    rhs._format = {};
    rhs._usage = {};
    rhs._image = {};
    rhs._views = {};
    rhs._allocation = {};
    rhs._defaultView = {};
    rhs._owned = {};

    return *this;
}

VkExtent3D VulkanImage::GetExtent() const
{
    return _extent;
}

VkFormat VulkanImage::GetFormat() const
{
    return _format;
}

VkImageUsageFlags VulkanImage::GetUsage() const
{
    return _usage;
}

VkImageLayout VulkanImage::GetLayout() const
{
    return _layout;
}

VkImage VulkanImage::Get() const
{
    return _image;
}

VkImageView VulkanImage::GetDefaultView() const
{
    return _defaultView;
}

VkImageView VulkanImage::CreateView(VkImageAspectFlags viewAspectMask, uint32_t mipLevels)
{
    VkComponentMapping componentMapping = {};
    componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = viewAspectMask;
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

    VkImageView view = VK_NULL_HANDLE;
    VK_CHECK(vkCreateImageView(_device->Get(), &viewCreateInfo, nullptr, &view))

    _views.push_back(view);

    return view;
}

void VulkanImage::DestroyViews()
{
    for (auto view : _views)
        vkDestroyImageView(_device->Get(), view, nullptr);
}

void VulkanImage::UploadData(const std::span<const std::byte> data, VkImageLayout finalLayout)
{

    // Create a staging buffer.
    VmaAllocationInfo allocInfo = {};
    VulkanBuffer stagingBuffer { _device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, (VkDeviceSize)data.size(), &allocInfo, true };

    // Copy image memory to the staging buffer.
    std::memcpy(allocInfo.pMappedData, data.data(), data.size());

    _device->ImmediateSubmit([&](VkCommandBuffer cmd) {
        Transition(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy region {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = _extent;

        vkCmdCopyBufferToImage(cmd, stagingBuffer.Get(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    });

    if (_mipLevels > 1)
        GenerateMipmaps();

    Transition(finalLayout);
}

void VulkanImage::Transition(VkCommandBuffer cmd, VkImageLayout layout)
{
    if (_layout == layout) {
        return; // How wonderful, we already are in the layout!
    }

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
    } else if (_layout == VK_IMAGE_LAYOUT_UNDEFINED && layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    } else if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::runtime_error("Unsupported VulkanImage layout transition!");
    }

    barrier.oldLayout = _layout;
    barrier.newLayout = layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(
        cmd,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    _layout = layout;
}

void VulkanImage::Transition(VkImageLayout layout)
{
    _device->ImmediateSubmit([&](VkCommandBuffer cmd) {
        Transition(cmd, layout);
    });
}

void VulkanImage::Transition(
    VkCommandBuffer                 cmd, 
    VkPipelineStageFlags            srcStageFlags, 
    VkPipelineStageFlags            dstStageFlags, 
    VkImageLayout                   newLayout,
    VkAccessFlags                   srcAccessMask, 
    VkAccessFlags                   dstAccessMask, 
    const VkImageSubresourceRange&  subresourceRange)
{
    VkImageMemoryBarrier2 barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = srcStageFlags;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstStageMask = dstStageFlags;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = _layout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;
    barrier.subresourceRange = subresourceRange;

    VkDependencyInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;
    info.dependencyFlags = 0;
    info.memoryBarrierCount = 0;
    info.pMemoryBarriers = nullptr;
    info.bufferMemoryBarrierCount = 0;
    info.pBufferMemoryBarriers = nullptr;
    info.imageMemoryBarrierCount = 1;
    info.pImageMemoryBarriers = &barrier;

    vkCmdPipelineBarrier2(cmd, &info);

    _layout = newLayout;
}

void VulkanImage::GenerateMipmaps()
{
    VkFormatProperties properties = _device->GetPhysicalDevice()->GetFormatProperties(_format);

    if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    _device->ImmediateSubmit([this](VkCommandBuffer cmd){
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = _extent.width;
        int32_t mipHeight = _extent.height;

        for (uint32_t i = 1; i < _mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, {}, 0, nullptr, 0, nullptr, 1, &barrier);

            VkImageBlit region = {};
            region.srcSubresource = {};
            region.srcOffsets[0] = { 0, 0, 0 };
            region.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            region.dstOffsets[0] = { 0, 0, 0 };
            region.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 }; // Divide by 2 if possible.

            region.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            region.srcSubresource.mipLevel       = i - 1;
            region.srcSubresource.baseArrayLayer = 0;
            region.srcSubresource.layerCount     = 1;

            region.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            region.dstSubresource.mipLevel       = i;
            region.dstSubresource.baseArrayLayer = 0;
            region.dstSubresource.layerCount     = 1;

            vkCmdBlitImage(cmd, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, {}, 0, nullptr, 0, nullptr, 1, &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
        barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, {}, 0, nullptr, 0, nullptr, 1, &barrier);
    });

    // After uploading all mipmaps the image's layout is considered shader read only.
    _layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

} // namespace bl
