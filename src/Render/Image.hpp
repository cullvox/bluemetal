#pragma once

#include "Render/RenderDevice.hpp"

namespace bl {

class Image 
{
public:
    Image();
    Image(RenderDevice& renderDevice, VkImageType type, VkFormat format, Extent2D extent, uint32_t mipLevels, VkImageUsageFlags usage);
    ~Image();   
    Image& operator=(const Image& rhs) = delete;
    Image& operator=(Image&& rhs) noexcept;
    VkImage getImage() const noexcept;
    VkImageView getImageView() const noexcept;
private:
    RenderDevice* m_pRenderDevice;
    VkImage m_image;
    VmaAllocation m_allocation;
};

} // namespace bl