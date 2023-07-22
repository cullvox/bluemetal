#pragma once

#include "Math/Vector2.h"
#include "Window/Window.h"
#include "Device.h"

namespace bl
{

class BLUEMETAL_API Swapchain
{
public:
    static const VkFormat DEFAULT_FORMAT = VK_FORMAT_R8G8B8A8_SRGB; 
    static const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; 
    static const VkPresentModeKHR DEFAULT_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR; 

    Swapchain(std::shared_ptr<Device> device, std::shared_ptr<Window> window, VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR);
    ~Swapchain();

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
    bool isMailboxSupported() { return m_isMailboxSupported; }
    bool isImmediateSupported() { return m_isImmediateSupported; }

private:  
    void ensureSurfaceSupported();
    void chooseImageCount();
    void chooseFormat();
    void choosePresentMode(VkPresentModeKHR presentMode);
    void chooseExtent();
    void obtainImages();
    void createImageViews();
    void destroyImageViews();
    
    std::shared_ptr<Device>     m_device;
    std::shared_ptr<Window>     m_window;
    uint32_t                    m_imageCount;
    VkSurfaceFormatKHR          m_surfaceFormat;
    VkPresentModeKHR            m_presentMode;
    VkExtent2D                  m_extent;
    VkSwapchainKHR              m_swapchain;
    std::vector<VkImage>        m_swapImages;
    std::vector<VkImageView>    m_swapImageViews;
    bool                        m_isMailboxSupported;
    bool                        m_isImmediateSupported;
};

} // namespace bl