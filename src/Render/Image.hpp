#pragma once

#include "Render/RenderDevice.hpp"

namespace bl {

class Image 
{
public:
    Image(RenderDevice& renderDevice, VkImageType type, VkFormat format, Extent2D extent, int32_t mipLevels, VkImageUsageFlags usage);
    ~Image();

private:
    RenderDevice& m_renderDevice;
    VkImage m_image;
    VmaAllocation m_allocation;
};

} // namespace bl