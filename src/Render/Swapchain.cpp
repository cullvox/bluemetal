//===========================//
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
//===========================//
#include "config/Config.hpp"
#include "Swapchain.hpp"
//===========================//



CSwapchain::CSwapchain(CConfig* pConfig, CRenderDevice* pRenderDevice, IWindow* pWindow)
    : IConfigurable(pConfig), m_pRenderDevice(pRenderDevice)
{
}

CSwapchain::~CSwapchain()
{
    vkDestroySwapchainKHR(m_pRenderDevice->GetDevice(), m_swapchain, nullptr);
    vkDestroySurfaceKHR(m_pRenderDevice->GetInstance(), m_surface, nullptr);
}

void CSwapchain::OnConfigReceive(const std::vector<SConfigValue>& values)
{

    uint32_t pQueueFamilyIndices[2] = { 
        m_pRenderDevice->GetGraphicsFamilyIndex(), 
        m_pRenderDevice->GetPresentFamilyIndex() 
    };

    EnsureSurfaceSupported();
    m_imageCount = GetImageCount();    
    m_surfaceFormat = FindSurfaceFormat();
    m_presentMode = FindPresentMode();
    m_surface = m_pWindow->CreateSurface(m_pRenderDevice->GetInstance());

    /* Create the window swapchain. */
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = m_surface;
    createInfo.minImageCount = m_imageCount;
    createInfo.imageFormat = m_surfaceFormat.format;
    createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    createInfo.imageExtent = m_pWindow->GetExtent();
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageSharingMode = m_pRenderDevice->AreQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = m_pRenderDevice->AreQueuesSame() ? 1 : 2;
    createInfo.pQueueFamilyIndices = pQueueFamilyIndices;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    if (vkCreateSwapchainKHR(m_pRenderDevice->GetDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't create a vulkan swapchain!");
    }
}

void CSwapchain::EnsureSurfaceSupported()
{
    /* Our surface should be supported... if not thats a sin. */
    VkBool32 supported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(m_pRenderDevice->GetPhysicalDevice(), m_pRenderDevice->GetPresentFamilyIndex(), m_pWindow->GetSurface(), &supported) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get vulkan surface support!");
    }
}

uint32_t CSwapchain::GetImageCount()
{
        /* Get the minimum image count to be used. */
    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pRenderDevice->GetPhysicalDevice(), m_pWindow->GetSurface(), &capabilities);

    uint32_t m_imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

VkSurfaceFormatKHR CSwapchain::FindSurfaceFormat()
{

    static const VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /* Retrieve the formats from vulkan. */
    uint32_t surfaceFormatsCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_pRenderDevice->GetPhysicalDevice(), m_pWindow->GetSurface(), &surfaceFormatsCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not find a vulkan surface format!");
    }
    
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    surfaceFormats.resize(surfaceFormatsCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(m_pRenderDevice->GetPhysicalDevice(), m_pWindow->GetSurface(), &surfaceFormatsCount, surfaceFormats.data());

    /* Find the surface format/colorspace to be used. */
    auto it = std::find(surfaceFormats.begin(), surfaceFormats.end(), defaultFormat);
    if (it != surfaceFormats.end())
    {
        m_surfaceFormat = *it;
        return;
    }

    /* For fallback use the first one we find... */
    m_surfaceFormat = surfaceFormats.front();
}