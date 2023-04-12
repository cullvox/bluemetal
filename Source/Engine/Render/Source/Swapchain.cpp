#include "Core/Log.hpp"
#include "Core/Precompiled.hpp"
#include "Render/Swapchain.hpp"

blSwapchain::blSwapchain(blWindow* window, const RenderDevice* renderDevice)
    : _window(window)
    , _renderDevice(renderDevice)
{

    // If the surface was not created yet, just do it automatically.
    if (_window->getVulkanSurface() == VK_NULL_HANDLE)
        _window->createVulkanSurface(_renderDevice->getInstance());
    
    ensureSurfaceSupported();

    recreate();
}

blSwapchain::~blSwapchain() noexcept
{
    vkDestroySwapchainKHR();
}

VkSwapchainKHR blSwapchain::getSwapchain() const noexcept
{
    return _swapchain;
}

VkSurfaceKHR blSwapchain::getSurface() const noexcept
{
    return _surface;
}

VkFormat blSwapchain::getFormat() const noexcept
{
    return _surfaceFormat.format;
}

blExtent2D blSwapchain::getExtent() const noexcept
{
    return blExtent2D{_extent.width, _extent.height};
}

uint32_t blSwapchain::getImageCount() const noexcept
{
    return _imageCount;
}

const std::vector<VkImage>& blSwapchain::getImages() const noexcept
{
    return _swapImages;
}

void blSwapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& recreated)
{
    imageIndex = 0;
    recreated = false;

    VkResult result = vkAcquireNextImageKHR(_pRenderDevice->getDevice(), _swapchain, UINT64_MAX, semaphore, fence, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate();
        recreated = true;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Could not acquire the next swapchain image!");
    }
}

void blSwapchain::ensureSurfaceSupported()
{
    /* Query the device for surface support. */
    VkBool32 supported = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(_pRenderDevice->getPhysicalDevice(), _pRenderDevice->getPresentFamilyIndex(), _surface, &supported) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get vulkan physical device surface support!");
        return false;
    }

    if (!supported)
    {
        throw std::runtime_error("Surface was not supported for swapchain/present device!");
    }
}

void blSwapchain::chooseImageCount()
{
    /* Get the minimum image count to be used. */
    VkSurfaceCapabilitiesKHR capabilities = {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_pRenderDevice->getPhysicalDevice(), _surface, &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't check physical device image count!");
    }

    _imageCount = 
        (capabilities.minImageCount + 1 <= capabilities.maxImageCount) 
            ? capabilities.minImageCount + 1 
            : capabilities.maxImageCount;
}

void blSwapchain::chooseFormat()
{
    static const VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    /* Retrieve the formats from vulkan. */
    uint32_t surfaceFormatsCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(_pRenderDevice->getPhysicalDevice(), _surface, &surfaceFormatsCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not find a vulkan surface format!");
    }
    
    std::vector<VkSurfaceFormatKHR> surfaceFormats{};
    surfaceFormats.resize(surfaceFormatsCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(_pRenderDevice->getPhysicalDevice(), _surface, &surfaceFormatsCount, surfaceFormats.data());

    /* Find the surface format/colorspace to be used. */
    auto foundMode = std::find_if(surfaceFormats.begin(), surfaceFormats.end(), [](VkSurfaceFormatKHR format){ if (format.colorSpace == defaultFormat.colorSpace && format.format == defaultFormat.format) {return true;} else  {return false;} });
    
    if (foundMode != surfaceFormats.end())
    {
        _surfaceFormat = defaultFormat;
        return;
    }

    /* As fallback use the first format... */
    _surfaceFormat = surfaceFormats.front();
}

void blSwapchain::choosePresentMode(VkPresentModeKHR requestedPresentMode)
{

    /* Obtain the present modes from the physical device. */
    uint32_t presentModesCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(_pRenderDevice->getPhysicalDevice(), _surface, &presentModesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan physical device surface formats!");
    }

    std::vector<VkPresentModeKHR> presentModes{};
    presentModes.resize(presentModesCount);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(_pRenderDevice->getPhysicalDevice(), _surface, &presentModesCount, presentModes.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan physical device surface formats!");
    }

    _presentMode = VK_PRESENT_MODE_FIFO_KHR; // If our mode wasn't found just use FIFO, default that's always available.

    /* Find if we have the requested present mode. */
    if (std::find(presentModes.begin(), presentModes.end(), requestedPresentMode) != presentModes.end())
        _presentMode = requestedPresentMode;
}

void blSwapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities{};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_pRenderDevice->getPhysicalDevice(), _surface, &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan physical device surface capabilities!")
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        _extent = capabilities.currentExtent;
    }
    else
    {
        const Extent2D extent = _pWindow->getExtent();
        _extent = VkExtent2D{
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void blSwapchain::obtainImages()
{
    if (vkGetSwapchainImagesKHR(_pRenderDevice->getDevice(), _swapchain, &_imageCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan swapchain images count!\n");
    }

    _swapImages.resize(_imageCount);

    if (vkGetSwapchainImagesKHR(_pRenderDevice->getDevice(), _swapchain, &_imageCount, _swapImages.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the vulkan swapchain images!\n");
    }
}

bool blSwapchain::recreate() 
{
    vkDeviceWaitIdle(_pRenderDevice->getDevice());

    vkDestroySwapchain(_pRenderDevice->getDevice(), VK_NULL_HANDLE, _swapchain);
    
    chooseImageCount();
    chooseFormat();
    chooseExtent();

    if (_extent.width == 0 || _extent.height == 0)
    {
        BL_LOG(blLogType::eInfo, "Swapchain recreation requires a window resize greater than 0x0.");
        return;
    }

    /* Build the create info structure for the swapchain. */
    const uint32_t pQueueFamilyIndices[2] = { 
        _pRenderDevice->getGraphicsFamilyIndex(), 
        _pRenderDevice->getPresentFamilyIndex() 
    };

    const VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = _surface,
        .minImageCount = _imageCount,
        .imageFormat = _surfaceFormat.format,
        .imageColorSpace = _surfaceFormat.colorSpace,
        .imageExtent = VkExtent2D{_extent.width, _extent.height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = _pRenderDevice->areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = _pRenderDevice->areQueuesSame() ? 1U : 2U,
        .pQueueFamilyIndices = pQueueFamilyIndices,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR, // _presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    if (vkCreateSwapchainKHR(_pRenderDevice->getDevice(), &createInfo, nullptr, &_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't create a vulkan swapchain!\n");
    }

    obtainImages();
}