#pragma once

#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"
#include <vulkan/vulkan.hpp>

class BLUEMETAL_RENDER_API blSwapchain
{
public:
    static const vk::Format DEFAULT_FORMAT = vk::Format::eR8G8B8A8Srgb;
    static const vk::ColorSpaceKHR DEFAULT_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;
    static const vk::PresentModeKHR DEFAULT_PRESENT_MODE = vk::PresentModeKHR::eMailbox;

    explicit blSwapchain(std::shared_ptr<blRenderDevice> renderDevice, vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo);

    std::vector<vk::PresentModeKHR> getPresentModes();
    void recreate(vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo);
    vk::Format getFormat() const noexcept;
    vk::Extent2D getExtent() const noexcept;
    uint32_t getImageCount() const noexcept;
    std::vector<vk::Image> getImages() const noexcept;
    std::vector<vk::ImageView> getImageViews() const noexcept;
    std::vector<vk::Framebuffer> getFramebuffers() const noexcept;
    vk::SwapchainKHR getSwapchain() noexcept;
    void acquireNext(vk::Semaphore semaphore, vk::Fence fence, uint32_t& imageIndex, bool& recreated);
    bool isMailboxSupported() const noexcept { return _isMailboxSupported; }
    bool isImmediateSupported() const noexcept { return _isImmediateSupported; }

private:  
    void ensureSurfaceSupported();
    void chooseImageCount();
    void chooseFormat();
    void choosePresentMode(vk::PresentModeKHR presentMode);
    void chooseExtent();
    void obtainImages();
    void createImageViews();
    void destroyImageViews();
    
    std::shared_ptr<blRenderDevice>     _renderDevice;
    uint32_t                            _imageCount;
    vk::SurfaceFormatKHR                _surfaceFormat;
    vk::PresentModeKHR                  _presentMode;
    vk::Extent2D                        _extent;
    vk::UniqueSwapchainKHR              _swapChain;
    std::vector<vk::Image>              _swapImages;
    std::vector<vk::UniqueImageView>    _swapImageViews;
    bool                                _isMailboxSupported;
    bool                                _isImmediateSupported;
};