#pragma once

#include "Vulkan.h"

namespace bl {

class VulkanDevice;
class VulkanImage;

class VulkanImageView {
    VulkanDevice* _device;
    VkImageView _imageView;
public:
    VulkanImageView(VulkanDevice* device, VulkanImage* image, VkImageViewType viewType, VkFormat format, VkComponentMapping components, VkImageSubresourceRange subresourceRange);
    VulkanImageView(VulkanImageView&& view);
    VulkanImageView(VulkanImageView& view) = delete;
    ~VulkanImageView();

    VulkanImageView& operator=(VulkanImageView&& view);
    VulkanImageView& operator=(VulkanImageView& view) = delete;

    VkImageView Get();
};

}