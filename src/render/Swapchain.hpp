#pragma once

#include "config/Config.hpp"
#include "Window.hpp"
#include "RenderDevice.hpp"
#include "vulkan/vulkan_core.h"

class CSwapchain : public IConfigurable
{
public:
    CSwapchain(CConfig* = g_pConfig.get(), CRenderDevice* = g_pRenderDevice.get(), IWindow* = g_pWindow.get());
    ~CSwapchain();

    VkSwapchainKHR  GetSwapchain() const noexcept;
    VkSurfaceKHR    GetSurface() const noexcept;

    void            OnConfigReceive(const std::vector<SConfigValue>&) override;
private:
    void                EnsureSurfaceSupported();
    uint32_t            GetImageCount();
    VkSurfaceFormatKHR  FindSurfaceFormat();
    VkPresentModeKHR    FindPresentMode();

    CRenderDevice*      m_pRenderDevice;
    IWindow*            m_pWindow;
    uint32_t            m_imageCount;
    VkSurfaceFormatKHR  m_surfaceFormat;
    VkPresentModeKHR    m_presentMode;
    VkSwapchainKHR      m_swapchain;
};