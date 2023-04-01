#pragma once

#include "Render/RenderDevice.hpp"

namespace bl 
{

class BLOODLUST_RENDER_API Image
{
public:
    Image() noexcept;
    Image(RenderDevice& renderDevice, VkImageType type, VkFormat format, Extent2D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask) noexcept;
    ~Image() noexcept;
    
    Image& operator=(Image&& rhs) noexcept;

    [[nodiscard]] bool good() const noexcept;
    [[nodiscard]] VkImage getImage() const noexcept;
    [[nodiscard]] VkImageView getImageView() const noexcept;

private:
    void collapse() noexcept;

    RenderDevice* m_pRenderDevice;
    VkImage m_image;
    VkImageView m_imageView;
    VmaAllocation m_allocation;
};

} // namespace bl