#pragma once

#include "Math/Vector3.h"
#include "Device.h"

namespace bl
{

class BLUEMETAL_API Image
{
public:
    Image(std::shared_ptr<Device> device, VkImageType type, VkFormat format, Extent3D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    ~Image();

    Extent3D getExtent();
    VkFormat getFormat();
    VkImageUsageFlags getUsage();
    VkImage getImage();
    VkImageView getImageView();

private:
    std::shared_ptr<Device> m_device;
    Extent3D                m_extent;
    VkImageType             m_type;
    VkFormat                m_format;
    VkImageUsageFlags       m_usage;
    VkImage                 m_image;
    VkImageView             m_imageView;
    VmaAllocation           m_allocation;
};

} // namespace bl