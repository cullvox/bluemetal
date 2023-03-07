#include "Render/Swapchain.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace bl {

Swapchain::Swapchain(Window& window, RenderDevice& renderDevice)
    : m_window(window), m_renderDevice(renderDevice), m_surface(VK_NULL_HANDLE), m_imageCount(0)
    , m_surfaceFormat({}), m_presentMode(VK_PRESENT_MODE_FIFO_KHR), m_swapchain(VK_NULL_HANDLE)
{
    spdlog::info("Creating vulkan swapchain.");
    
    if (!m_window.createVulkanSurface(m_renderDevice.getInstance(), m_surface))
    {
        throw std::runtime_error("Could not create the vulkan surface!");
    }
    ensureSurfaceSupported();
    recreateSwapchain();
}

Swapchain::~Swapchain()
{
    spdlog::info("Destroying vulkan swapchain.");
    destroySwapchain();
    vkDestroySurfaceKHR(m_renderDevice.getInstance(), m_surface, nullptr);
}

VkSwapchainKHR Swapchain::getSwapchain() const noexcept
{
    return m_swapchain;
}

VkSurfaceKHR Swapchain::getSurface() const noexcept
{
    return m_surface;
}

VkFormat Swapchain::getColorFormat() const noexcept
{
    return m_surfaceFormat.format;
}

Extent2D Swapchain::getSwapchainExtent()
{
    return m_extent;
}

uint32_t Swapchain::getImageCount() const noexcept
{
    return m_imageCount;
}

const std::vector<VkImage>& Swapchain::getSwapchainImages() const noexcept
{
    return m_swapImages;
}

void Swapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& wasRecreated)
{
    VkResult result = vkAcquireNextImageKHR(m_renderDevice.getDevice(), m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain();
        wasRecreated = true;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Could not acquire swap chain image!");
    }
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

void Swapchain::findImageCount()
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

void Swapchain::findPresentMode()
{

    /* Obtain the present modes from the physical device. */
    uint32_t presentModesCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_renderDevice.getPhysicalDevice(), m_surface, &presentModesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan physical device surface formats!");
    }

    std::vector<VkPresentModeKHR> presentModes{};
    presentModes.resize(presentModesCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_renderDevice.getPhysicalDevice(), m_surface, &presentModesCount, presentModes.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan physical device surface formats!");
    }

    /* Find if we have the requested present mode. */
    auto foundMode = std::find(presentModes.begin(), presentModes.end(), DEFAULT_PRESENT_MODE);
    if (foundMode == presentModes.end()) m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    else m_presentMode = DEFAULT_PRESENT_MODE;

}

void Swapchain::findExtent()
{
    VkSurfaceCapabilitiesKHR capabilities{};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_renderDevice.getPhysicalDevice(), m_surface, &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan physical device surface capabilities!");
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        m_extent = Extent2D{capabilities.currentExtent.width, capabilities.currentExtent.height};
    }
    else
    {
        const Extent2D extent = m_window.getExtent();
        m_extent = Extent2D{
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void Swapchain::getImages()
{
    if (vkGetSwapchainImagesKHR(m_renderDevice.getDevice(), m_swapchain, &m_imageCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan swapchain images count!");
    }

    m_swapImages.resize(m_imageCount);

    if (vkGetSwapchainImagesKHR(m_renderDevice.getDevice(), m_swapchain, &m_imageCount, m_swapImages.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan swapchain images!");
    }
}

void Swapchain::destroySwapchain()
{
    vkDestroySwapchainKHR(m_renderDevice.getDevice(), m_swapchain, nullptr);
}

void Swapchain::recreateSwapchain()
{
    if (m_swapchain)
    {
        destroySwapchain();
    }

    findImageCount();
    findSurfaceFormat();
    findPresentMode();
    findExtent();

    /* Build the create info structure for the swapchain. */
    const uint32_t pQueueFamilyIndices[2] = { 
        m_renderDevice.getGraphicsFamilyIndex(), 
        m_renderDevice.getPresentFamilyIndex() 
    };

    const VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_surface,
        .minImageCount = m_imageCount,
        .imageFormat = m_surfaceFormat.format,
        .imageColorSpace = m_surfaceFormat.colorSpace,
        .imageExtent = VkExtent2D{m_extent.width, m_extent.height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = m_renderDevice.areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = m_renderDevice.areQueuesSame() ? 1U : 2U,
        .pQueueFamilyIndices = pQueueFamilyIndices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR, // m_presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (vkCreateSwapchainKHR(m_renderDevice.getDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't create a vulkan swapchain!");
    }

    spdlog::info("Resizing Swapchain: \n\tExtent2D: {{ Width: {}, Height: {} }}", m_extent.width, m_extent.height);

    getImages();
}

} /* namespace bl*/