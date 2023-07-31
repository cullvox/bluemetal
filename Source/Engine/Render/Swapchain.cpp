#include "Swapchain.h"
#include "Core/Log.h"

namespace bl
{

Swapchain::Swapchain(
    std::shared_ptr<Device>             device, 
    std::shared_ptr<Window>             window, 
    std::optional<VkPresentModeKHR>     presentMode,
    std::optional<VkSurfaceFormatKHR>   surfaceFormat)
    : m_physicalDevice(device->getPhysicalDevice())
    , m_device(device)
    , m_window(window)
    , m_imageCount(0)
    , m_surfaceFormat({})
    , m_presentMode(VK_PRESENT_MODE_FIFO_KHR)
    , m_extent({})
    , m_swapchain(VK_NULL_HANDLE)
    , m_swapImages({})
    , m_swapImageViews({})
    , m_isMailboxSupported(false)
    , m_isImmediateSupported(false)
{
    ensureSurfaceSupported();
    recreate(presentMode, surfaceFormat);
}

Swapchain::~Swapchain()
{
    destroyImageViews();
    vkDestroySwapchainKHR(m_device->getHandle(), m_swapchain, nullptr);
}

VkSwapchainKHR Swapchain::getHandle()
{
    return m_swapchain;
}

VkFormat Swapchain::getFormat() 
{
    return m_surfaceFormat.format;
}

VkExtent2D Swapchain::getExtent() 
{
    return m_extent;
}

uint32_t Swapchain::getImageCount() 
{
    return m_imageCount;
}

VkPresentModeKHR Swapchain::getPresentMode()
{
    return m_presentMode;
}

VkSurfaceFormatKHR Swapchain::getSurfaceFormat()
{
    return m_surfaceFormat;
}

std::vector<VkImageView> Swapchain::getImageViews() 
{
    return m_swapImageViews;
}

std::vector<VkImage> Swapchain::getImages() 
{
    return m_swapImages;
}

void Swapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex, bool* pRecreated)
{
    *pImageIndex = 0;
    *pRecreated = false;

    VkResult result = vkAcquireNextImageKHR(m_device->getHandle(), m_swapchain, UINT32_MAX, semaphore, fence, pImageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate();
        *pRecreated = true;
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Could not acquire the next swapchain image!");
    }
}

void Swapchain::ensureSurfaceSupported()
{
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice->getHandle(), m_device->getPresentFamilyIndex(), m_window->getSurface(), &supported);

    if (!supported)
    {
        throw std::runtime_error("Could not ensure Vulkan surface support on physical device!");
    }
}

void Swapchain::chooseImageCount()
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice->getHandle(), m_window->getSurface(), &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan surface capabilities!");
    }

    // Try to at least get one over the minimum
    m_imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

void Swapchain::chooseFormat(std::optional<VkSurfaceFormatKHR> format)
{
 
    // Get the surface formats from the physical device.
    std::vector<VkSurfaceFormatKHR> formats = m_physicalDevice->getSurfaceFormats(m_window);

    // find the surface format/colorspace to be used
    VkSurfaceFormatKHR toFind = format.value_or(DEFAULT_SURFACE_FORMAT);

    auto pred = [&toFind](const VkSurfaceFormatKHR& sf)
                { 
                    return sf.format == toFind.format && sf.colorSpace == toFind.colorSpace; 
                };

    auto it = std::find_if(formats.begin(), formats.end(), pred);

    if (it != formats.end())
    {
        m_surfaceFormat = toFind;
        return;
    }

    // as fallback use the first format
    m_surfaceFormat = formats.front();
    BL_LOG(LogType::eWarning, "Could not find Vulkan default surface format: ({}, {}), falling back to: ({}, {}).", string_VkFormat(toFind.format), string_VkColorSpaceKHR(toFind.colorSpace), string_VkFormat(m_surfaceFormat.format), string_VkColorSpaceKHR(m_surfaceFormat.colorSpace));
}

void Swapchain::choosePresentMode(std::optional<VkPresentModeKHR> presentMode)
{
    // Obtain the present modes from the physical device.
    std::vector<VkPresentModeKHR> modes = m_physicalDevice->getPresentModes(m_window);

    // Fill up the present meta info.
    m_isMailboxSupported = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end();
    m_isImmediateSupported = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != modes.end();

    // Try to find the caller's present mode or use the default instead.    
    auto toFind = presentMode.value_or(DEFAULT_PRESENT_MODE);
    auto it = std::find(modes.begin(), modes.end(), toFind);

    if (it != modes.end())
    {
        m_presentMode = toFind;
        return;
    }
    
    BL_LOG(LogType::eWarning, "Present mode {} is unavailable, using VK_PRESENT_MODE_FIFO_KHR.", string_VkPresentModeKHR(DEFAULT_PRESENT_MODE));

    // If our mode wasn't found just use FIFO, as that's always available.
    m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void Swapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = {};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice->getHandle(), m_window->getSurface(), &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface capabilities!");
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        m_extent = capabilities.currentExtent;
    }
    else
    {
        Extent2D extent = m_window->getExtent();
        m_extent = VkExtent2D{
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void Swapchain::obtainImages()
{
    uint32_t imageCount = 0;
    if (vkGetSwapchainImagesKHR(m_device->getHandle(), m_swapchain, &imageCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan swapchain image count!");
    }

    m_swapImages.resize(imageCount);

    if (vkGetSwapchainImagesKHR(m_device->getHandle(), m_swapchain, &imageCount, m_swapImages.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan swapchain images!");
    }
}

void Swapchain::createImageViews()
{
    m_swapImageViews.resize(m_imageCount);

    for (uint32_t i = 0; i < m_imageCount; i++)
    {
        VkComponentMapping componentMapping = {};
        componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
        componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY; 
        componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY; 
        componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY; 
        
        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = 1;

        VkImageViewCreateInfo imageViewInfo = {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = nullptr;
        imageViewInfo.flags = 0;
        imageViewInfo.image = m_swapImages[i];
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = m_surfaceFormat.format;
        imageViewInfo.components = componentMapping;
        imageViewInfo.subresourceRange = subresourceRange;

        if (vkCreateImageView(m_device->getHandle(), &imageViewInfo, nullptr, &m_swapImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a Vulkan image view for the swapchain!");
        }
    }
}

void Swapchain::destroyImageViews()
{
    for (uint32_t i = 0; i < m_swapImageViews.size(); i++)
    {
        vkDestroyImageView(m_device->getHandle(), m_swapImageViews[i], nullptr);
    }

    m_swapImageViews.clear();
}

void Swapchain::recreate(
    std::optional<VkPresentModeKHR>     presentMode,
    std::optional<VkSurfaceFormatKHR>   surfaceFormat) 
{
    // Wait for the device to finish doing it's things.
    m_device->waitForDevice();

    // Destroy the previous swapchain and it's image views.
    vkDestroySwapchainKHR(m_device->getHandle(), m_swapchain, nullptr);
    destroyImageViews();

    // Begin recreating the swapchain.
    chooseImageCount();
    chooseFormat(surfaceFormat);
    chooseExtent();
    choosePresentMode(presentMode);

    if (m_extent.width == 0 || m_extent.height == 0)
    {
        BL_LOG(LogType::eInfo, "Swapchain recreation requires a window resize greater than 0x0.");
        return;
    }

    // swapchains use both graphics and present, provide it the queue families
    std::vector queueFamilyIndices = 
    { 
        m_device->getGraphicsFamilyIndex(), 
        m_device->getPresentFamilyIndex() 
    };

    // if they are the same we don't want to misreport
    queueFamilyIndices.resize(m_device->areQueuesSame() ? 1 : 2);

    // do these out of the create info initialization
    // create the swapchain
    auto imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    auto imageSharingMode = m_device->areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_EXCLUSIVE;
    
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = {};
    createInfo.surface = m_window->getSurface();
    createInfo.minImageCount = m_imageCount;
    createInfo.imageFormat = m_surfaceFormat.format;
    createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    createInfo.imageExtent = (VkExtent2D)m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = imageUsage;
    createInfo.imageSharingMode = imageSharingMode;
    createInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_device->getHandle(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan swapchain!");
    }

    obtainImages();
    createImageViews();
}

} // namespace bl