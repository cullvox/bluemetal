#pragma once

#include "Export.h"
#include "Render/RenderDevice.hpp"
#include "Render/Buffer.hpp"

class BLUEMETAL__API blImage
{
public:
    blImage(std::shared_ptr<blRenderDevice> renderDevice, vk::ImageType type, vk::Format format, blExtent2D extent, uint32_t mipLevels, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask);

    blExtent2D getExtent() const> noexcept;
    vk::Format getFormat() const noexcept;
    vk::ImageUsageFlags getUsage() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;

private:
    std::shared_ptr<blRenderDevice> _renderDevice;
    blExtent2D                      _extent;
    vk::ImageType                   _type;
    vk::Format                      _format;
    vk::ImageUsageFlags             _usage;
    vk::Image                       _image;
    vk::UniqueImageView             _imageView;
    std::unique_ptr<VmaAllocation>  _allocation;
};