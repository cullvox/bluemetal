#pragma once

namespace bl {

class VulkanDevice;
class VulkanRenderer;
class VulkanImage;
class VulkanImageView;

class VulkanRenderImage {
    std::vector<VulkanImage> _images;
    std::vector<VulkanImageView> _imageViews;
    VkFormat _format;
    VkExtent2D _extent;
    VkSampleCountFlagBits _samples;

public:
    VulkanRenderImage(VulkanDevice* device, VulkanRenderer* renderer, VkFormat format, VkExtent2D extent, VkSampleCountFlagBits samples);
    ~VulkanRenderImage();

    VkImage GetImage(uint32_t currentFrame) const;
    VkImageView GetImageView(uint32_t currentFrame) const;
    VkFormat GetFormat() const;
    VkExtent2D GetExtent() const;
    VkSampleCountFlagBits GetSampleCount() const;

    std::span<VulkanImageView> GetImageViews() const;
};

}