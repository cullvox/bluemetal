#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Buffer.hpp"
#include "Render/Export.h"

class BLOODLUST_RENDER_API blImage
{
public:
    blImage() noexcept;
    blImage(const blRenderDevice* renderDevice, VkImageType type, 
        VkFormat format, blExtent2D extent, uint32_t mipLevels,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    ~blImage() noexcept;

    blExtent2D getExtent() const noexcept;
    VkFormat getFormat() const noexcept;
    VkImageUsageFlags getUsage() const noexcept;
    VkImage getImage() const noexcept;
    VkImageView getImageView() const noexcept;

private:
    const blRenderDevice*   _pRenderDevice;
    blExtent2D              _extent;
    VkFormat                _format;
    VkImageUsageFlags       _usage;

    VkImage                 _image;
    VmaAllocation           _allocation;
    VkImageView             _imageView;
};