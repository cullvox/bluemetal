#pragma once

#include "Render/RenderDevice.hpp"
#include <vulkan/vulkan_core.h>

namespace bl {

class Image 
{
public:
    Image();
    Image(RenderDevice& renderDevice, VkImageType type, VkFormat format, Extent2D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    ~Image();   
    Image& operator=(const Image& rhs) = delete;
    Image& operator=(Image&& rhs) noexcept;
    VkImage getImage() const noexcept;
    VkImageView getImageView() const noexcept;
private:
    void destroy();

    RenderDevice* m_pRenderDevice;
    VkImage m_image;
    VkImageView m_imageView;
    VmaAllocation m_allocation;
};

} // namespace bl