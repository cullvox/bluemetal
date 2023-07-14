#include "Swapchain.h"
#include "Core/Log.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

blSwapchain::blSwapchain(std::shared_ptr<blRenderDevice> renderDevice, VkPresentModeKHR presentMode)
    : _renderDevice(renderDevice)
{
    ensureSurfaceSupported();
    recreate(presentModes);
}

VkSwapchainKHR blSwapchain::getSwapchain() noexcept
{
    return _swapChain;
}

VkFormat blSwapchain::getFormat() const noexcept
{
    return _surfaceFormat.format;
}

blExtent2D blSwapchain::getExtent() const noexcept
{
    return (blExtent2D)_extent;
}

uint32_t blSwapchain::getImageCount() const noexcept
{
    return _imageCount;
}

std::vector<VkImageView> blSwapchain::getImageViews() const noexcept
{
    return _swapImageViews;
}

std::vector<VkImage> blSwapchain::getImages() const noexcept
{
    return _swapImages;
}

void blSwapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& recreated)
{
    recreated = false;

    VkResult result = vkAcquireNextImageKHR(_renderDevice->getDevice(), _swapChain, UINT32_MAX, semaphore, fence, &imageIndex)
    
    if (result != VK_OUT_OF_DATE_KHR)
    {
        recreate();
        recreated = true();
    }
    else
    {
        BL_LOG(blLogType::eFatal, "Could not acquire the next swapchain image!")
    }
}

void blSwapchain::ensureSurfaceSupported()
{
    VkBool32 supported = _renderDevice->getPhysicalDevice().getSurfaceSupportKHR(_renderDevice->getPresentFamilyIndex(), _renderDevice->getSurface());

    if (!supported)
    {
        BL_LOG(blLogType::eFatal, "Surface was not supported for physical device!");
    }
}

void blSwapchain::chooseImageCount()
{
    VkSurfaceCapabilitiesKHR capabilities = _renderDevice->getPhysicalDevice().getSurfaceCapabilitiesKHR(_renderDevice->getSurface());

    // Try to at least get one over the minimum
    _imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

void blSwapchain::chooseFormat()
{
    static constexpr VkSurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    std::vector<VkSurfaceFormatKHR> surfaceFormats = _renderDevice->getPhysicalDevice().getSurfaceFormatsKHR(_renderDevice->getSurface());

    // find the surface format/colorspace to be used
    if (std::find_if(surfaceFormats.begin(), surfaceFormats.end(),
        [](VkSurfaceFormatKHR format)
        { 
            return (format.colorSpace == defaultFormat.colorSpace) && (format.format == defaultFormat.format);
        }) != surfaceFormats.end())
    {
        _surfaceFormat = defaultFormat;
        return;
    }

    // as fallback use the first format
    _surfaceFormat = surfaceFormats.front();
}

void blSwapchain::choosePresentMode(VkPresentModeKHR requestedPresentMode)
{
    // Obtain the present modes from the physical device.
    std::vector<VkPresentModeKHR> presentModes = _renderDevice->getPhysicalDevice().getSurfacePresentModesKHR(_renderDevice->getSurface());

    // Fill up the present meta info
    _isMailboxSupported = std::find(presentModes.begin(), presentModes.end(), VkPresentModeKHR::eMailbox) != presentModes.end();
    _isImmediateSupported = std::find(presentModes.begin(), presentModes.end(), VkPresentModeKHR::eImmediate) != presentModes.end();

    // Find if we have the requested present mode.
    if (std::find(presentModes.begin(), presentModes.end(), requestedPresentMode) != presentModes.end())
    {
        _presentMode = requestedPresentMode;
        return;
    }    

    // If our mode wasn't found just use FIFO, that's always available.
    _presentMode = VkPresentModeKHR::eFifo; 
    BL_LOG(blLogType::eWarning, "Present mode {} is unavailable, using FIFO.", Vkto_string(requestedPresentMode));
}

void blSwapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = 
        _renderDevice->getPhysicalDevice()
            .getSurfaceCapabilitiesKHR(_renderDevice->getSurface());


    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        _extent = capabilities.currentExtent;
    }
    else
    {
        const blExtent2D extent = _renderDevice->getWindow()->getExtent();
        _extent = VkExtent2D{
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void blSwapchain::obtainImages()
{
    _swapImages = _renderDevice->getDevice().getSwapchainImagesKHR(getSwapchain());
}

void blSwapchain::createImageViews()
{
    std::for_each(_swapImages.begin(), _swapImages.end(),
        [&](VkImage image)
        {

            const VkComponentMapping componentMapping
            {
                VkComponentSwizzle::eIdentity, // r
                VkComponentSwizzle::eIdentity, // g
                VkComponentSwizzle::eIdentity, // b
                VkComponentSwizzle::eIdentity  // a
            };

            const VkImageSubresourceRange subresourceRange
            {
                VkImageAspectFlagBits::eColor,    // aspectMask
                0,                                  // baseMipLevel
                1,                                  // levelCount
                0,                                  // baseArrayLayers
                1                                   // layerCount
            };

            const VkImageViewCreateInfo imageViewInfo
            {
                {},                         // flags
                image,                      // image
                VkImageViewType::e2D,     // format
                _surfaceFormat.format,      // viewType
                componentMapping,           // components
                subresourceRange            // subresourceRange
            };

            _swapImageViews.push_back(_renderDevice->getDevice().createImageViewUnique(imageViewInfo));
        });
}


void blSwapchain::recreate(VkPresentModeKHR presentMode) 
{
    // wait for the device to finish doing it's things
    _renderDevice->waitForDevice();

    // destroy the previous swapchain
    _renderDevice->getDevice().destroySwapchainKHR(_swapChain.release());

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

    std::vector queueFamilyIndices = 
    { 
        _renderDevice->getGraphicsFamilyIndex(), 
        _renderDevice->getPresentFamilyIndex() 
    };

    queueFamilyIndices.resize(_renderDevice->areQueuesSame() ? 1 : 2);

    const VkSwapchainCreateInfoKHR createInfo
    {
        {},                                         // flags 
        _renderDevice->getSurface(),                // surface
        _imageCount,                                // minImageCount
        _surfaceFormat.format,                      // imageFormat
        _surfaceFormat.colorSpace,                  // imageColorSpace
        VkExtent2D{ _extent.width, _extent.height }, // imageExtent 
        1,                                          // imageArrayLayers
        VkImageUsageFlagBits::eColorAttachment | VkImageUsageFlagBits::eTransferDst, // imageUsage
        _renderDevice->areQueuesSame() ? VkSharingMode::eExclusive : VkSharingMode::eConcurrent, // imageSharingMode 
        queueFamilyIndices,                         // queueFamilyIndices
        VkSurfaceTransformFlagBitsKHR::eIdentity, // preTransform 
        VkCompositeAlphaFlagBitsKHR::eOpaque,     // compositeAlpha
        _presentMode,                               // presentMode
        VK_TRUE,                                    // clipped
        VK_NULL_HANDLE                              // oldSwapchain
    };

    _swapChain = _renderDevice->getDevice().createSwapchainKHRUnique(createInfo);

    obtainImages();
    createImageViews();
}

