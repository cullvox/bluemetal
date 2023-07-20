#pragma once

#include "Math/Vector2.h"
#include "Window/Window.h"
#include "Device.h"

class BLUEMETAL_API blSwapchain
{
public:
    static const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB; 
    static const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; 
    static const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR; 

    explicit blSwapchain(std::shared_ptr<blDevice> device, std::shared_ptr<blWindow> window, VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR);

    std::vector<VkPresentModeKHR> getPresentModes();
    void recreate(VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR);
    VkFormat getFormat();
    VkExtent2D getExtent();
    uint32_t getImageCount();
    std::vector<VkImage> getImages();
    std::vector<VkImageView> getImageViews();
    std::vector<VkFramebuffer> getFramebuffers();
    VkSwapchainKHR getSwapchain();
    void acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& recreated);
    bool isMailboxSupported() { return _isMailboxSupported; }
    bool isImmediateSupported() { return _isImmediateSupported; }

private:  
    void ensureSurfaceSupported();
    void chooseImageCount();
    void chooseFormat();
    void choosePresentMode(VkPresentModeKHR presentMode);
    void chooseExtent();
    void obtainImages();
    void createImageViews();
    void destroyImageViews();
    
    std::shared_ptr<blDevice>   _device;
    std::shared_ptr<blWindow>   _window;
    uint32_t                    _imageCount;
    VkSurfaceFormatKHR          _surfaceFormat;
    VkPresentModeKHR            _presentMode;
    VkExtent2D                  _extent;
    VkSwapchainKHR              _swapChain;
    std::vector<VkImage>        _swapImages;
    std::vector<VkImageView>    _swapImageViews;
    bool                        _isMailboxSupported;
    bool                        _isImmediateSupported;
};