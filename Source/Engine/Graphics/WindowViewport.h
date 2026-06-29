#pragma once

#include "Viewport.h"
#include "VulkanConfig.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanSwapchain.h"
#include <vulkan/vulkan_core.h>

namespace bl 
{

class Window;
class Renderer;
class VulkanSwapchain;

class WindowViewport : public Viewport
{
    Renderer* _renderer;
    Window* _window;
    
    std::unique_ptr<VulkanSwapchain> _swapchain;
    VkSurfaceKHR _surface;

    uint32_t _imageIndex;
    std::array<VkSemaphore, VulkanConfig::maxFramesInFlight> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkSemaphoreWaitInfo> _waitForSemaphores;
    std::vector<VulkanImage> _swapchainImages;
    std::vector<VulkanImageView> _swapchainImageViews;

protected:
    virtual void RecreateImages() override;

public:
    WindowViewport(Renderer* renderer, Window* window);
    ~WindowViewport();

    Window* GetWindow();
    VkPresentModeKHR GetPresentMode();
    virtual void PrepareForFrame(RenderData& rd) override;
    virtual void TransitionPreRender(RenderData& rd) override;
    virtual void TransitionPostRender(RenderData& rd) override;
    virtual void TransitionPrePresent(RenderData& rd) override;
    virtual void QueuePresent(RenderData& rd) override;

    virtual VkImageView GetRenderedImageView() override;

    virtual VkImageView GetColorRenderImageView();
    virtual VkImageView GetColorRenderResolveImageView();

    virtual void GetColorRenderingAttachments(std::vector<VkRenderingAttachmentInfo>& attachments) override;
    virtual void FillColorRenderingAttachmentsForUI(std::vector<VkRenderingAttachmentInfo>& attachments) override;
};

} // namespace bl