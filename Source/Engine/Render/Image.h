#pragma once

#include "Math/Vector3.h"
#include "Device.h"

class BLUEMETAL_API blImage
{
public:
    blImage(std::shared_ptr<blDevice> device, VkImageType type, VkFormat format, blExtent3D extent, uint32_t mipLevels, VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    ~blImage();

    blExtent3D getExtent() const noexcept;
    VkFormat getFormat() const noexcept;
    VkImageUsageFlags getUsage() const noexcept;
    VkImage getImage() const noexcept;
    VkImageView getImageView() const noexcept;

private:
    std::shared_ptr<blDevice>   _device;
    blExtent3D                  _extent;
    VkImageType                 _type;
    VkFormat                    _format;
    VkImageUsageFlags           _usage;
    VkImage                     _image;
    VkImageView                 _imageView;
    VmaAllocation               _allocation;
};