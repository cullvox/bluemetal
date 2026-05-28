#pragma once

#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>

namespace bl {

class VulkanDevice;
class VulkanImage;
class VulkanImageView;
class Renderer;
class VulkanRenderImage;
class RenderData;

class VulkanViewport {

    std::unique_ptr<VulkanImage> _renderImage;

public:
    VulkanViewport(VulkanDevice* device, Renderer* renderer, VkExtent2D extent);
    ~VulkanViewport();

    void SetExtent(VkExtent2D extent);
    VulkanImage* GetImage();
};


} // namespace bl