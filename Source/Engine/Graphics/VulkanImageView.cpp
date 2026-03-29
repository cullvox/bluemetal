#include "VulkanImageView.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

namespace bl {

VulkanImageView::VulkanImageView(VulkanDevice* device, VulkanImage* image, VkImageViewType viewType, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresourceRange)
    : _device(device)
{
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.image = image->Get();
    createInfo.viewType = viewType;
    createInfo.format = format;
    createInfo.components = components;
    createInfo.subresourceRange = subresourceRange;

    VK_CHECK(vkCreateImageView(_device->Get(), &createInfo, nullptr, &_imageView))
}

VulkanImageView::VulkanImageView(VulkanImageView&& from)
{
    _device = from._device;
    _imageView = from._imageView;

    from._device = nullptr;
    from._imageView = VK_NULL_HANDLE;
}

VulkanImageView::~VulkanImageView()
{
    vkDestroyImageView(_device->Get(), _imageView, nullptr);
}

VulkanImageView& VulkanImageView::operator=(VulkanImageView&& from)
{
    if (_imageView) {
        vkDestroyImageView(_device->Get(), _imageView, nullptr);
    }

    _device = from._device;
    _imageView = from._imageView;

    from._device = nullptr;
    from._imageView = VK_NULL_HANDLE;
    return *this;
}

VkImageView VulkanImageView::Get()
{
    return _imageView;
}

}