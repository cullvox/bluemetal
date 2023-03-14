#include "Core/Log.hpp"
#include "Render/Swapchain.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace bl 
{

Swapchain::Swapchain() noexcept
    : m_pWindow(nullptr)
    , m_pRenderDevice(nullptr)
    , m_surface(VK_NULL_HANDLE)
    , m_imageCount(0)
    , m_surfaceFormat({})
    , m_presentMode(VK_PRESENT_MODE_FIFO_KHR)
    , m_swapchain(VK_NULL_HANDLE)
{
}

Swapchain::Swapchain(Window& window, RenderDevice& renderDevice) noexcept
    : Swapchain()
{
    m_pWindow = &window;
    m_pRenderDevice = &renderDevice;

    if (not m_pWindow->createVulkanSurface(m_pRenderDevice->getInstance(), m_surface))
    {
        Logger::Error("Could not create the vulkan surface!");
        return;
    }
    
    if (not isSurfaceSupported() ||
        not recreate())
    {
        Logger::Error("Could not create the swapchain!\n");
        goto failureCleanup;
    }

    return;

failureCleanup:
    vkDestroySurfaceKHR(m_pRenderDevice->getInstance(), m_surface, nullptr);
}

Swapchain::~Swapchain() noexcept
{
    // Destroy the swapchain itself and the surface.
    collapse();

    if (m_surface) vkDestroySurfaceKHR(m_pRenderDevice->getInstance(), m_surface, nullptr);
}

Swapchain& Swapchain::operator=(Swapchain&& rhs) noexcept
{
    m_pWindow = rhs.m_pWindow;
    m_pRenderDevice = rhs.m_pRenderDevice;
    m_surface = rhs.m_surface;
    m_imageCount = rhs.m_imageCount;
    m_surfaceFormat = rhs.m_surfaceFormat;
    m_presentMode = rhs.m_presentMode;
    m_swapchain = rhs.m_swapchain;

    rhs.m_pWindow = nullptr;
    rhs.m_pRenderDevice = nullptr;
    rhs.m_surface = VK_NULL_HANDLE;
    rhs.m_imageCount = 0;
    rhs.m_surfaceFormat = {};
    rhs.m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    rhs.m_swapchain = VK_NULL_HANDLE;

    return *this;
}

bool Swapchain::good() const noexcept
{
    return m_surface != VK_NULL_HANDLE || m_swapchain != VK_NULL_HANDLE;
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
    return Extent2D{m_extent.width, m_extent.height};
}

uint32_t Swapchain::getImageCount() const noexcept
{
    return m_imageCount;
}

const std::vector<VkImage>& Swapchain::getImages() const noexcept
{
    return m_swapImages;
}

bool Swapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& requiresRecreation) noexcept
{
    if (not good())
    {
        Logger::Error("Could not acquire next, swapchain was not good.\n");
        requiresRecreation = true;
        return false;
    }

    requiresRecreation = false;

    VkResult result = vkAcquireNextImageKHR(m_pRenderDevice->getDevice(), m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        requiresRecreation = true;
        return true; // I'm not considering this an error in this context because its not fatal.
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        Logger::Error("Could not acquire the next swapchain image!\n");
        return false;
    }

    return true;
}

bool Swapchain::isSurfaceSupported() noexcept
{
    /* Query the device for surface support. */
    VkBool32 supported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(m_pRenderDevice->getPhysicalDevice(), m_pRenderDevice->getPresentFamilyIndex(), m_surface, &supported) != VK_SUCCESS)
    {
        Logger::Error("Could not get vulkan physical device surface support!\n");
        return false;
    }

    return supported;
}

bool Swapchain::chooseImageCount() noexcept
{
    /* Get the minimum image count to be used. */
    VkSurfaceCapabilitiesKHR capabilities = {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pRenderDevice->getPhysicalDevice(), m_surface, &capabilities) != VK_SUCCESS)
    {
        return false;
    }

    m_imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
    return true;
}

bool Swapchain::chooseFormat() noexcept
{
    static const VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /* Retrieve the formats from vulkan. */
    uint32_t surfaceFormatsCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_pRenderDevice->getPhysicalDevice(), m_surface, &surfaceFormatsCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not find a vulkan surface format!\n");
        return false;
    }
    
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    surfaceFormats.resize(surfaceFormatsCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(m_pRenderDevice->getPhysicalDevice(), m_surface, &surfaceFormatsCount, surfaceFormats.data());

    /* Find the surface format/colorspace to be used. */
    auto foundMode = std::find_if(surfaceFormats.begin(), surfaceFormats.end(), [](VkSurfaceFormatKHR format){ if (format.colorSpace == defaultFormat.colorSpace && format.format == defaultFormat.format) {return true;} else  {return false;} });
    
    if (foundMode != surfaceFormats.end())
    {
        m_surfaceFormat = defaultFormat;
        return true;
    }

    /* As fallback use the first format... */
    m_surfaceFormat = surfaceFormats.front();
    return true;
}

bool Swapchain::choosePresentMode(VkPresentModeKHR requestedPresentMode) noexcept
{

    /* Obtain the present modes from the physical device. */
    uint32_t presentModesCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_pRenderDevice->getPhysicalDevice(), m_surface, &presentModesCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan physical device surface formats!\n");
        return false;
    }

    std::vector<VkPresentModeKHR> presentModes{};
    presentModes.resize(presentModesCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_pRenderDevice->getPhysicalDevice(), m_surface, &presentModesCount, presentModes.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan physical device surface formats!\n");
        return false;
    }

    m_presentMode = VK_PRESENT_MODE_FIFO_KHR;

    /* Find if we have the requested present mode. */
    auto foundMode = std::find(presentModes.begin(), presentModes.end(), requestedPresentMode);

    if (foundMode != presentModes.end()) 
        m_presentMode = requestedPresentMode;

    return true;
}

bool Swapchain::chooseExtent() noexcept
{
    VkSurfaceCapabilitiesKHR capabilities{};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pRenderDevice->getPhysicalDevice(), m_surface, &capabilities) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan physical device surface capabilities!\n");
        return false;
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        m_extent = capabilities.currentExtent;
    }
    else
    {
        const Extent2D extent = m_pWindow->getExtent();
        m_extent = VkExtent2D{
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    return true;
}

bool Swapchain::obtainImages() noexcept
{
    if (vkGetSwapchainImagesKHR(m_pRenderDevice->getDevice(), m_swapchain, &m_imageCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan swapchain images count!\n");
        return false;
    }

    m_swapImages.resize(m_imageCount);

    if (vkGetSwapchainImagesKHR(m_pRenderDevice->getDevice(), m_swapchain, &m_imageCount, m_swapImages.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not get the vulkan swapchain images!\n");
        return false;
    }

    return true;
}

void Swapchain::collapse() noexcept
{
    if (m_swapchain) vkDestroySwapchainKHR(m_pRenderDevice->getDevice(), m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
}

bool Swapchain::recreate() noexcept
{
    vkDeviceWaitIdle(m_pRenderDevice->getDevice());

    if (good()) collapse();
    
    if (not chooseImageCount() ||
        not chooseFormat() ||
        not chooseExtent())
    {
        return false;
    }

    if (m_extent.width == 0 || m_extent.height == 0)
    {
        Logger::Log("Could not recreate a swapchain with an invalid extent! Waiting for a resize...\n");
        return false;
    }

    /* Build the create info structure for the swapchain. */
    const uint32_t pQueueFamilyIndices[2] = { 
        m_pRenderDevice->getGraphicsFamilyIndex(), 
        m_pRenderDevice->getPresentFamilyIndex() 
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
        .imageSharingMode = m_pRenderDevice->areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = m_pRenderDevice->areQueuesSame() ? 1U : 2U,
        .pQueueFamilyIndices = pQueueFamilyIndices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR, // m_presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (vkCreateSwapchainKHR(m_pRenderDevice->getDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        Logger::Error("Couldn't create a vulkan swapchain!\n");
        return false;
    }

    if (not obtainImages())
    {
        collapse();
        return false;
    }

#if BLOODLUST_DEBUG
    g_checkedGood = false;
#endif

    return true;
}

} /* namespace bl*/