#include "Swapchain.h"
#include "Core/Log.h"

namespace bl
{

Swapchain::Swapchain(std::shared_ptr<Device> device, std::shared_ptr<Window> window, VkPresentModeKHR presentMode)
    : m_device(device)
    , m_window(window)
{
    ensureSurfaceSupported();
    recreate(presentMode);
}

Swapchain::~Swapchain()
{
    vkDestroySwapchainKHR(m_device->getDevice(), m_swapchain, nullptr);
}

VkSwapchainKHR Swapchain::getSwapchain()
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

std::vector<VkImageView> Swapchain::getImageViews() 
{
    return m_swapImageViews;
}

std::vector<VkImage> Swapchain::getImages() 
{
    return m_swapImages;
}

void Swapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& recreated)
{
    recreated = false;

    VkResult result = vkAcquireNextImageKHR(m_device->getDevice(), m_swapchain, UINT32_MAX, semaphore, fence, &imageIndex);
    
    if (result != VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate();
        recreated = true;
    }
    else
    {
        throw std::runtime_error("Could not acquire the next swapchain image!");
    }
}

void Swapchain::ensureSurfaceSupported()
{
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_device->getPhysicalDevice(), m_device->getPresentFamilyIndex(), m_window->getSurface(), &supported);

    if (!supported)
    {
        throw std::runtime_error("Could not ensure Vulkan surface support on physical device!");
    }
}

void Swapchain::chooseImageCount()
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->getPhysicalDevice(), m_window->getSurface(), &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan surface capabilities!");
    }

    // Try to at least get one over the minimum
    m_imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

void Swapchain::chooseFormat()
{
    static constexpr VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    // get the surface formats from the physical device
    uint32_t formatCount = 0;
    std::vector<VkSurfaceFormatKHR> formats;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->getPhysicalDevice(), m_window->getSurface(), &formatCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("could not get Vulkan physical device surface formats!");
    }

    formats.resize(formatCount);
    
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->getPhysicalDevice(), m_window->getSurface(), &formatCount, formats.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("could not get Vulkan physical device surface formats!");
    }

    // find the surface format/colorspace to be used
    if (std::find_if(formats.begin(), formats.end(),
        [](VkSurfaceFormatKHR format)
        { 
            return (format.colorSpace == defaultFormat.colorSpace) && (format.format == defaultFormat.format);
        }) != formats.end())
    {
        m_surfaceFormat = defaultFormat;
        return;
    }

    // as fallback use the first format
    BL_LOG(LogType::eInfo, "Could not find Vulkan default surface format: ({}, {}), falling back to: ({}, {}).", string_VkFormat(defaultFormat.format), string_VkColorSpaceKHR(defaultFormat.colorSpace), string_VkFormat(m_surfaceFormat.format), string_VkColorSpaceKHR(m_surfaceFormat.colorSpace));
    m_surfaceFormat = formats.front();
}

void Swapchain::choosePresentMode(VkPresentModeKHR requestedPresentMode)
{
    // Obtain the present modes from the physical device.
    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->getPhysicalDevice(), m_window->getSurface(), &presentModeCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface present mode count!");
    }

    presentModes.resize(presentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->getPhysicalDevice(), m_window->getSurface(), &presentModeCount, presentModes.data()))
    {
        throw std::runtime_error("Could not get Vulkan physical device surface present modes!");
    }

    // Fill up the present meta info
    m_isMailboxSupported = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end();
    m_isImmediateSupported = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != presentModes.end();

    // Find if we have the requested present mode.
    if (std::find(presentModes.begin(), presentModes.end(), requestedPresentMode) != presentModes.end())
    {
        m_presentMode = requestedPresentMode;
        return;
    }    

    // If our mode wasn't found just use FIFO, that's always available.
    m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    BL_LOG(LogType::eWarning, "Present mode {} is unavailable, using FIFO.", string_VkPresentModeKHR(requestedPresentMode));
}

void Swapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = {};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->getPhysicalDevice(), m_window->getSurface(), &capabilities) != VK_SUCCESS)
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
    if (vkGetSwapchainImagesKHR(m_device->getDevice(), m_swapchain, &imageCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan swapchain image count!");
    }

    m_swapImages.resize(imageCount);

    if (vkGetSwapchainImagesKHR(m_device->getDevice(), m_swapchain, &imageCount, m_swapImages.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan swapchain images!");
    }
}

void Swapchain::createImageViews()
{
    m_swapImageViews.resize(m_swapImages.size());

    for (uint32_t i = 0; i < m_swapImages.size(); i++)
    {
        const VkComponentMapping componentMapping
        {
            VK_COMPONENT_SWIZZLE_IDENTITY, // r
            VK_COMPONENT_SWIZZLE_IDENTITY, // g
            VK_COMPONENT_SWIZZLE_IDENTITY, // b
            VK_COMPONENT_SWIZZLE_IDENTITY  // a
        };

        const VkImageSubresourceRange subresourceRange
        {
            VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask
            0,                          // baseMipLevel
            1,                          // levelCount
            0,                          // baseArrayLayers
            1                           // layerCount
        };

        const VkImageViewCreateInfo imageViewInfo
        {
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // sType
            nullptr,                                    // pNext
            0,                                          // flags
            m_swapImages[i],                            // image
            VK_IMAGE_VIEW_TYPE_2D,                      // viewType
            m_surfaceFormat.format,                     // format
            componentMapping,                           // components
            subresourceRange                            // subresourceRange
        };

        if (vkCreateImageView(m_device->getDevice(), &imageViewInfo, nullptr, &m_swapImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a Vulkan image view for the swapchain!");
        }
    }
}


void Swapchain::recreate(VkPresentModeKHR presentMode) 
{
    // wait for the device to finish doing it's things
    m_device->waitForDevice();

    // destroy the previous swapchain
    vkDestroySwapchainKHR(m_device->getDevice(), m_swapchain, nullptr);

    m_swapImageViews.clear();

    chooseImageCount();
    chooseFormat();
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

    if (vkCreateSwapchainKHR(m_device->getDevice(), &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan swapchain!");
    }

    obtainImages();
    createImageViews();
}

} // namespace bl