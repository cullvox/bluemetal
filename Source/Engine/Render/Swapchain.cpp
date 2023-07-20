#include "Swapchain.h"
#include "Core/Log.h"

blSwapchain::blSwapchain(std::shared_ptr<blDevice> device, std::shared_ptr<blWindow> window, VkPresentModeKHR presentMode)
    : _device(device)
    , _window(window)
{
    ensureSurfaceSupported();
    recreate(presentMode);
}

VkSwapchainKHR blSwapchain::getSwapchain()
{
    return _swapChain;
}

VkFormat blSwapchain::getFormat() 
{
    return _surfaceFormat.format;
}

VkExtent2D blSwapchain::getExtent() 
{
    return _extent;
}

uint32_t blSwapchain::getImageCount() 
{
    return _imageCount;
}

std::vector<VkImageView> blSwapchain::getImageViews() 
{
    return _swapImageViews;
}

std::vector<VkImage> blSwapchain::getImages() 
{
    return _swapImages;
}

void blSwapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& recreated)
{
    recreated = false;

    VkResult result = vkAcquireNextImageKHR(_device->getDevice(), _swapChain, UINT32_MAX, semaphore, fence, &imageIndex);
    
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

void blSwapchain::ensureSurfaceSupported()
{
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device->getPhysicalDevice(), _device->getPresentFamilyIndex(), _window->getSurface(), &supported);

    if (!supported)
    {
        throw std::runtime_error("Could not ensure Vulkan surface support on physical device!");
    }
}

void blSwapchain::chooseImageCount()
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device->getPhysicalDevice(), _window->getSurface(), &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan surface capabilities!");
    }

    // Try to at least get one over the minimum
    _imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

void blSwapchain::chooseFormat()
{
    static constexpr VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    // get the surface formats from the physical device
    uint32_t formatCount = 0;
    std::vector<VkSurfaceFormatKHR> formats;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(_device->getPhysicalDevice(), _window->getSurface(), &formatCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("could not get Vulkan physical device surface formats!");
    }

    formats.resize(formatCount);
    
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(_device->getPhysicalDevice(), _window->getSurface(), &formatCount, formats.data()) != VK_SUCCESS)
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
        _surfaceFormat = defaultFormat;
        return;
    }

    // as fallback use the first format
    BL_LOG(blLogType::eInfo, "Could not find Vulkan default surface format: ({}, {}), falling back to: ({}, {}).", string_VkFormat(defaultFormat.format), string_VkColorSpaceKHR(defaultFormat.colorSpace), string_VkFormat(_surfaceFormat.format), string_VkColorSpaceKHR(_surfaceFormat.colorSpace));
    _surfaceFormat = formats.front();
}

void blSwapchain::choosePresentMode(VkPresentModeKHR requestedPresentMode)
{
    // Obtain the present modes from the physical device.
    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(_device->getPhysicalDevice(), _window->getSurface(), &presentModeCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface present mode count!");
    }

    presentModes.resize(presentModeCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(_device->getPhysicalDevice(), _window->getSurface(), &presentModeCount, presentModes.data()))
    {
        throw std::runtime_error("Could not get Vulkan physical device surface present modes!");
    }

    // Fill up the present meta info
    _isMailboxSupported = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end();
    _isImmediateSupported = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != presentModes.end();

    // Find if we have the requested present mode.
    if (std::find(presentModes.begin(), presentModes.end(), requestedPresentMode) != presentModes.end())
    {
        _presentMode = requestedPresentMode;
        return;
    }    

    // If our mode wasn't found just use FIFO, that's always available.
    _presentMode = VK_PRESENT_MODE_FIFO_KHR;
    BL_LOG(blLogType::eWarning, "Present mode {} is unavailable, using FIFO.", string_VkPresentModeKHR(requestedPresentMode));
}

void blSwapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = {};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device->getPhysicalDevice(), _window->getSurface(), &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device surface capabilities!");
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        _extent = capabilities.currentExtent;
    }
    else
    {
        const blExtent2D extent = _window->getExtent();
        _extent = VkExtent2D{
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void blSwapchain::obtainImages()
{
    uint32_t imageCount = 0;
    if (vkGetSwapchainImagesKHR(_device->getDevice(), _swapChain, &imageCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan swapchain image count!");
    }

    _swapImages.resize(imageCount);

    if (vkGetSwapchainImagesKHR(_device->getDevice(), _swapChain, &imageCount, _swapImages.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan swapchain images!");
    }
}

void blSwapchain::createImageViews()
{
    _swapImageViews.resize(_swapImages.size());

    for (uint32_t i = 0; i < _swapImages.size(); i++)
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
            _swapImages[i],                             // image
            VK_IMAGE_VIEW_TYPE_2D,                      // viewType
            _surfaceFormat.format,                      // format
            componentMapping,                           // components
            subresourceRange                            // subresourceRange
        };

        if (vkCreateImageView(_device->getDevice(), &imageViewInfo, nullptr, &_swapImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a Vulkan image view for the swapchain!");
        }
    }
}


void blSwapchain::recreate(VkPresentModeKHR presentMode) 
{
    // wait for the device to finish doing it's things
    _device->waitForDevice();

    // destroy the previous swapchain
    _device->getDevice().destroySwapchainKHR(_swapChain.release());

    _swapImageViews.clear();

    chooseImageCount();
    chooseFormat();
    chooseExtent();
    choosePresentMode(presentMode);

    if (_extent.width == 0 || _extent.height == 0)
    {
        BL_LOG(blLogType::eInfo, "Swapchain recreation requires a window resize greater than 0x0.");
        return;
    }

    // swapchains use both graphics and present, provide it the queue families
    std::vector queueFamilyIndices = 
    { 
        _device->getGraphicsFamilyIndex(), 
        _device->getPresentFamilyIndex() 
    };

    // if they are the same we don't want to misreport
    queueFamilyIndices.resize(_device->areQueuesSame() ? 1 : 2);

    // do these out of the create info initialization
    // create the swapchain
    auto imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    auto imageSharingMode = _device->areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_EXCLUSIVE;
    
    const VkSwapchainCreateInfoKHR createInfo
    {
        {},                                     // flags 
        _window->getSurface(),                  // surface
        _imageCount,                            // minImageCount
        _surfaceFormat.format,                  // imageFormat
        _surfaceFormat.colorSpace,              // imageColorSpace
        (VkExtent2D)_extent,                    // imageExtent 
        1,                                      // imageArrayLayers
        imageUsage,                             // imageUsage
        imageSharingMode,                       // imageSharingMode 
        queueFamilyIndices,                     // queueFamilyIndices
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,  // preTransform 
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,      // compositeAlpha
        _presentMode,                           // presentMode
        VK_TRUE,                                // clipped
        VK_NULL_HANDLE                          // oldSwapchain
    };

    if (vkCreateSwapchainKHR(_device->getDevice(), &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan swapchain!");
    }

    obtainImages();
    createImageViews();
}

