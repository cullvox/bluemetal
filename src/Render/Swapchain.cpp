//===========================//
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
//===========================//
#include "config/Config.hpp"
#include "Swapchain.hpp"
//===========================//

namespace bl {

Swapchain::Swapchain(Window& window, RenderDevice& renderDevice)
    : m_window(window), m_renderDevice(renderDevice)
{
    ensureSurfaceSupported();

    m_window.createVulkanSurface(m_renderDevice.getInstance(), m_surface);
}

Swapchain::~Swapchain()
{
    vkDestroySwapchainKHR(m_renderDevice.getDevice(), m_swapchain, nullptr);
    vkDestroySurfaceKHR(m_renderDevice.getInstance(), m_surface, nullptr);
}

void Swapchain::ensureSurfaceSupported()
{
    /* Our surface should be supported... if not thats a sin. */
    VkBool32 supported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(m_renderDevice.getPhysicalDevice(), m_renderDevice.getPresentFamilyIndex(), m_surface, &supported) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get vulkan surface support!");
    }
}

void Swapchain::getImageCount()
{
    /* Get the minimum image count to be used. */
    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_renderDevice.getPhysicalDevice(), m_surface, &capabilities);

    m_imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

void Swapchain::findSurfaceFormat()
{
    static const VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /* Retrieve the formats from vulkan. */
    uint32_t surfaceFormatsCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_renderDevice.getPhysicalDevice(), m_surface, &surfaceFormatsCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not find a vulkan surface format!");
    }
    
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    surfaceFormats.resize(surfaceFormatsCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(m_renderDevice.getPhysicalDevice(), m_surface, &surfaceFormatsCount, surfaceFormats.data());

    /* Find the surface format/colorspace to be used. */
    auto foundMode = std::find_if(surfaceFormats.begin(), surfaceFormats.end(), [](VkSurfaceFormatKHR format){ if (format.colorSpace == defaultFormat.colorSpace && format.format == defaultFormat.format) {return true;} else  {return false;} });
    
    if (foundMode != surfaceFormats.end())
    {
        m_surfaceFormat = defaultFormat;
        return;
    }

    /* As fallback use the first format... */
    m_surfaceFormat = surfaceFormats.front();
}



void Swapchain::recreateSwapchain()
{
    if (m_swapchain != VK_NULL_HANDLE) vkDestroySwapchainKHR(m_renderDevice.getDevice(), m_swapchain, nullptr);

    getImageCount();
    findSurfaceFormat();
    findPresentMode();

    /* Build the create info structure for the swapchain. */
    const uint32_t pQueueFamilyIndices[2] = { 
        m_renderDevice.getGraphicsFamilyIndex(), 
        m_renderDevice.getPresentFamilyIndex() 
    };

    const Extent2D windowExtent = m_window.getExtent();

    const VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_surface,
        .minImageCount = m_imageCount,
        .imageFormat = m_surfaceFormat.format,
        .imageColorSpace = m_surfaceFormat.colorSpace,
        .imageExtent = VkExtent2D{(unsigned int)windowExtent.width, (unsigned int)windowExtent.height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = m_renderDevice.areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = m_renderDevice.areQueuesSame() ? 1U : 2U,
        .pQueueFamilyIndices = pQueueFamilyIndices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = m_presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (vkCreateSwapchainKHR(m_renderDevice.getDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't create a vulkan swapchain!");
    }
}

} /* namespace bl*/