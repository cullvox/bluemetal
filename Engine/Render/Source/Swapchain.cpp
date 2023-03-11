#include "Core/Log.hpp"
#include "Render/Swapchain.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace bl 
{

#if BLOODLUST_DEBUG
    bool g_checkedGood;
#endif

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
}

Swapchain::~Swapchain()
{
    spdlog::info("Destroying vulkan swapchain.");
    destroy();
    vkDestroySurfaceKHR(m_renderDevice.getInstance(), m_surface, nullptr);
}

bool Swapchain::good() const noexcept
{
    return m_swapchain != VK_NULL_HANDLE;
}

VkSwapchainKHR Swapchain::getSwapchain() const noexcept
{
    return m_swapchain;
}

VkSurfaceKHR Swapchain::getSurface() const noexcept
{
    return m_surface;
}

VkFormat Swapchain::getFormat() const noexcept
{
    return m_surfaceFormat.format;
}

Extent2D Swapchain::getExtent() const noexcept
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

bool Swapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex) noexcept
{
    VkResult result = vkAcquireNextImageKHR(m_renderDevice.getDevice(), m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        Logger::Error("Could not acquire the next swapchain image!");
        return false;
    }

    return true;
}

bool Swapchain::isSurfaceSupported() noexcept
{
    /* Query the device for surface support. */
    VkBool32 supported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(m_renderDevice.getPhysicalDevice(), m_renderDevice.getPresentFamilyIndex(), m_surface, &supported) != VK_SUCCESS)
    {
        Logger::Error("Could not get vulkan physical device surface support!");
    }

    return supported;
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

bool Swapchain::chooseExtent()
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

bool Swapchain::getImages_REAL() noexcept
{
    if (vkGetSwapchainImagesKHR(m_renderDevice.getDevice(), m_swapchain, &m_imageCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan swapchain images count!");
        return false;
    }

    m_swapImages.resize(m_imageCount);

    if (vkGetSwapchainImagesKHR(m_renderDevice.getDevice(), m_swapchain, &m_imageCount, m_swapImages.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan swapchain images!");
        return false;
    }

    return true;
}

void Swapchain::destroy()
{
    vkDestroySwapchainKHR(m_renderDevice.getDevice(), m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
}

bool Swapchain::recreate()
{
    vkDeviceWaitIdle(m_renderDevice.getDevice());

    if (good()) destroy();
 
    // Ensure that the window size is greater than 0.
    Extent2D windowExtent = m_window.getExtent();
    if (windowExtent.width == 0 || windowExtent.height == 0)
    {
        Logger::Error("Tried to recreate swapchain with an invalid size.");
        return false;
    }
 
    
    findImageCount();
    findSurfaceFormat();
    findPresentMode();
    findExtent();

    if (m_extent.width == 0 || m_extent.height == 0)
    {
        Logger::Log("Could not recreate a swapchain with an invalid extent! Waiting for a resize...");
        return false;
    }

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
        Logger::Error("Couldn't create a vulkan swapchain!");
        return false;
    }

    getImages_REAL();

#if BLOODLUST_DEBUG
    g_checkedGood = false;
#endif
}

} /* namespace bl*/