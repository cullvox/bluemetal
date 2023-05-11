#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Buffer.hpp"
#include "Render/Export.h"

class BLUEMETAL_RENDER_API blImage
{
public:
    blImage(std::shared_ptr<blRenderDevice> renderDevice, VkImageType type, 
        VkFormat format, blExtent2D extent, uint32_t mipLevels,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    ~blImage() noexcept;

    blExtent2D getExtent() const noexcept;
    VkFormat getFormat() const noexcept;
    VkImageUsageFlags getUsage() const noexcept;
    VkImage getImage() const noexcept;
    VkImageView getImageView() const noexcept;

private:
    void build(const blRenderDevice* renderDevice, VkImageType type, 
        VkFormat format, blExtent2D extent, uint32_t mipLevels,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    void collapse() noexcept;

    std::shared_ptr<blRenderDevice> _renderDevice;
    blExtent2D              _extent;

    VkImageType             _type;
    VkFormat                _format;
    VkImageUsageFlags       _usage;
    VkImage                 _image;
    VmaAllocation           _allocation;
    VkImageView             _imageView;
};