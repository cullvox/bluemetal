#pragma once

#include <memory>

#include "Graphics/Vulkan.h"

namespace bl {

class RenderData;
class VulkanDevice;
class VulkanImage;
class VulkanImageView;
class VulkanBuffer;
class VulkanMaterialInstance;

class SelectionPass
{

    VulkanDevice*                       _device;
    VkExtent2D                          _extent;
    VulkanMaterialInstance*             _material;

    std::unique_ptr<VulkanImage>        _selectionImage;
    std::unique_ptr<VulkanImageView>    _selectionImageView;
    std::unique_ptr<VulkanBuffer>       _selectionBuffer;

    VkFormat                            _depthFormat;
    std::unique_ptr<VulkanImage>        _depthImage;
    std::unique_ptr<VulkanImageView>    _depthImageView;

public:
    SelectionPass(VulkanDevice* device, VkExtent2D extent);
    ~SelectionPass();

    void SetMaterial(VulkanMaterialInstance* instance);
    void GetColorFormats(std::vector<VkFormat>& formats);
    VkFormat GetDepthFormat();
    VkFormat GetStencilFormat();
    void Render(RenderData& data);
    void Resize(VkExtent2D newExtent);
};

}