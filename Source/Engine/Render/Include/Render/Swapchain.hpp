#pragma once

#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

class BLUEMETAL_RENDER_API blSwapchain
{
public:
    static const vk::Format DEFAULT_FORMAT = vk::Format::eR8G8B8A8Srgb;
    static const vk::ColorSpaceKHR DEFAULT_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;
    static const vk::PresentModeKHR DEFAULT_PRESENT_MODE = vk::PresentModeKHR::eMailbox;

    blSwapchain(std::shared_ptr<const blRenderDevice> renderDevice);
    ~blSwapchain() noexcept;

    void recreate();
    vk::Format getFormat() const noexcept;
    blExtent2D getExtent() const noexcept;
    vk::Extent2D getExtentVk() const noexcept;
    uint32_t getImageCount() const noexcept;
    const std::vector<vk::Image>& getImages() const noexcept;
    std::vector<vk::ImageView> getImageViews() const noexcept;
    const std::vector<vk::Framebuffer>& getFramebuffers() const noexcept;
    vk::SwapchainKHR getSwapchain() noexcept;
    void acquireNext(vk::Semaphore semaphore, vk::Fence fence, 
                        uint32_t& imageIndex, bool& recreated);

private:  
    void ensureSurfaceSupported();
    void chooseImageCount();
    void chooseFormat();
    void choosePresentMode(vk::PresentModeKHR requestedPresentMode = DEFAULT_PRESENT_MODE);
    void chooseExtent();
    void obtainImages();
    void createImageViews();
    void destroyImageViews();
    
    std::shared_ptr<const blRenderDevice>   _renderDevice;

    uint32_t                            _imageCount;
    vk::SurfaceFormatKHR                _surfaceFormat;
    vk::PresentModeKHR                  _presentMode;
    vk::Extent2D                        _extent;
    vk::UniqueSwapchainKHR              _swapchain;
    std::vector<vk::Image>              _swapImages;
    std::vector<vk::UniqueImageView>    _swapImageViews;
};