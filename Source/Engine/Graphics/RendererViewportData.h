#pragma once

#include <vulkan/vulkan_core.h>
namespace bl
{

class VulkanImage;
class VulkanImageView;
class VulkanBuffer;

class RendererViewportData
{
    std::unique_ptr<VulkanImage> _colorImage, _colorImageResolved, _selectionImage, _selectionImageResolved, _depthImage;
    std::unique_ptr<VulkanImageView> _colorImageView, _colorImageResolvedView, _selectionImageView, _selectionImageResolvedView, _depthImageView;
    std::unique_ptr<VulkanBuffer> _selectionBuffer;

    std::vector<VkRenderingAttachmentInfo> _colorAttachments;
    VkRenderingAttachmentInfo _depthAttachment;

public:
    RendererViewportData();
    ~RendererViewportData();

    VulkanImage* GetColorImage();
    VulkanImage* GetColorImageResolved();
    VulkanImage* GetSelectionImage();
    VulkanImage* GetSelectionImageResolved();
    VulkanImage* GetDepthImage();
    VulkanImageView* GetColorImageView();
    VulkanImageView* GetColorImageResolvedView();
    VulkanImageView* GetSelectionImageView();
    VulkanImageView* GetSelectionImageResolvedView();
    VulkanImageView* GetDepthImageView();
    std::span<const VkRenderingAttachmentInfo> GetColorAttachments();
    const VkRenderingAttachmentInfo& GetDepthAttachment();

};

} // namespace bl