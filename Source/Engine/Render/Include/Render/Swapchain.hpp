#pragma once

#include "Math/Vector2.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

class BLOODLUST_RENDER_API blSwapchain
{
public:
    static const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
    static const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    static const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

    blSwapchain(const std::shared_ptr<blWindow> window, 
        const std::shared_ptr<blRenderDevice> renderDevice);
    ~blSwapchain() noexcept;

    void recreate();
    VkFormat getFormat() const noexcept;
    blExtent2D getExtent() const noexcept;
    uint32_t getImageCount() const noexcept;
    const std::vector<VkImage>& getImages() const noexcept;
    VkSwapchainKHR getSwapchain() const noexcept;
    void acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex,
        bool& recreated);

private:  
    void ensureSurfaceSupported();
    void chooseImageCount();
    void chooseFormat();
    void choosePresentMode(VkPresentModeKHR requestedPresentMode = DEFAULT_PRESENT_MODE);
    void chooseExtent();
    void obtainImages();
    
    std::shared_ptr<blWindow>               _window;
    const std::shared_ptr<blRenderDevice>   _renderDevice;

    uint32_t                _imageCount;
    VkSurfaceFormatKHR      _surfaceFormat;
    VkPresentModeKHR        _presentMode;
    VkExtent2D              _extent;
    VkSwapchainKHR          _swapchain;
    std::vector<VkImage>    _swapImages;
};