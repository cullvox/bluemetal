#pragma once

#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"
#include <glm/ext/matrix_float4x4.hpp>

namespace bl {

class VulkanDevice;
class VulkanImage;
class VulkanImageView;
class Renderer;
class VulkanRenderImage;
class RenderData;

class VulkanViewport {

    VulkanRenderImage* _renderImage;

public:
    VulkanViewport(VulkanDevice* device, Renderer* renderer, VkExtent2D extent);
    ~VulkanViewport();

    void SetExtent(VkExtent2D extent);
    void SetOutputTexture(VulkanRenderImage& texture);
};


} // namespace bl