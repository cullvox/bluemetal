#include "Core/Log.hpp"
#include "Core/Precompiled.hpp"
#include "Render/Swapchain.hpp"

blSwapchain::blSwapchain(
    std::shared_ptr<const blRenderDevice> renderDevice)
    : _renderDevice(renderDevice)
{
    ensureSurfaceSupported();
    recreate();
}

blSwapchain::~blSwapchain() noexcept
{
}

vk::SwapchainKHR blSwapchain::getSwapchain() noexcept
{
    return _swapChain.get();
}

vk::Format blSwapchain::getFormat() const noexcept
{
    return _surfaceFormat.format;
}

blExtent2D blSwapchain::getExtent() const noexcept
{
    return blExtent2D{_extent.width, _extent.height};
}

vk::Extent2D blSwapchain::getExtentVk() const noexcept
{
    return _extent;
}

uint32_t blSwapchain::getImageCount() const noexcept
{
    return _imageCount;
}

std::vector<vk::ImageView> blSwapchain::getImageViews() const noexcept
{
    std::vector<vk::ImageView> swapImageViews;

    // Transform the unique image views into normal image views with get()
    std::transform(
        _swapImageViews.begin(), _swapImageViews.end(),
        std::back_inserter(swapImageViews),
        [](const vk::UniqueImageView& imageView)
        {
            return imageView.get();
        });

    return swapImageViews;
}

const std::vector<vk::Image>& blSwapchain::getImages() const noexcept
{
    return _swapImages;
}

void blSwapchain::acquireNext(vk::Semaphore semaphore, vk::Fence fence, uint32_t& imageIndex, bool& recreated)
{
    recreated = false;

    try
    {
        imageIndex = _renderDevice->getDevice().acquireNextImageKHR(getSwapchain(), std::numeric_limits<uint64_t>::max(), semaphore, fence).value;
    }
    catch (const vk::OutOfDateKHRError& e)
    {
        recreate();
        recreated = true;
    }
    catch (...)
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
    vk::SurfaceCapabilitiesKHR capabilities = _renderDevice->getPhysicalDevice().getSurfaceCapabilitiesKHR(_renderDevice->getSurface());

    // Try to get three maximum amount images.
    _imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.maxImageCount;
}

void blSwapchain::chooseFormat()
{
    static constexpr vk::SurfaceFormatKHR defaultFormat{DEFAULT_FORMAT, DEFAULT_COLOR_SPACE};

    std::vector<vk::SurfaceFormatKHR> surfaceFormats = 
        _renderDevice->getPhysicalDevice()
            .getSurfaceFormatsKHR(_renderDevice->getSurface());

    // Find the surface format/colorspace to be used.
    auto foundMode = std::find_if(surfaceFormats.begin(), surfaceFormats.end(),
        [](vk::SurfaceFormatKHR format)
        { 
            return (format.colorSpace == defaultFormat.colorSpace && format.format == defaultFormat.format);
        });
    
    if (foundMode != surfaceFormats.end())
    {
        _surfaceFormat = defaultFormat;
        return;
    }

    // As fallback use the first format.
    _surfaceFormat = surfaceFormats.front();
}

void blSwapchain::choosePresentMode(vk::PresentModeKHR requestedPresentMode)
{
    // Obtain the present modes from the physical device.
    std::vector<vk::PresentModeKHR> presentModes = _renderDevice->getPhysicalDevice().getSurfacePresentModesKHR(_renderDevice->getSurface());

    // Fill up the present meta info
    _isMailboxSupported = std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eMailbox) != presentModes.end();
    _isImmediateSupported = std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eImmediate) != presentModes.end();

    // Find if we have the requested present mode.
    if (std::find(presentModes.begin(), presentModes.end(), requestedPresentMode) != presentModes.end())
    {
        _presentMode = requestedPresentMode;
        return;
    }    

    // If our mode wasn't found just use FIFO, that's always available.
    _presentMode = vk::PresentModeKHR::eFifo; 
    BL_LOG(blLogType::eWarning, "Present mode {} is unavailable, using FIFO.", vk::to_string(requestedPresentMode));
}

void blSwapchain::chooseExtent()
{
    vk::SurfaceCapabilitiesKHR capabilities = 
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
        [&](vk::Image image)
        {

            const vk::ComponentMapping componentMapping
            {
                vk::ComponentSwizzle::eIdentity, // r
                vk::ComponentSwizzle::eIdentity, // g
                vk::ComponentSwizzle::eIdentity, // b
                vk::ComponentSwizzle::eIdentity  // a
            };

            const vk::ImageSubresourceRange subresourceRange
            {
                vk::ImageAspectFlagBits::eColor,    // aspectMask
                0,                                  // baseMipLevel
                1,                                  // levelCount
                0,                                  // baseArrayLayers
                1                                   // layerCount
            };

            const vk::ImageViewCreateInfo imageViewInfo
            {
                {},                         // flags
                image,                      // image
                vk::ImageViewType::e2D,     // format
                _surfaceFormat.format,      // viewType
                componentMapping,           // components
                subresourceRange            // subresourceRange
            };

            _swapImageViews.push_back(_renderDevice->getDevice().createImageViewUnique(imageViewInfo));
        });
}


void blSwapchain::recreate(vk::PresentModeKHR presentMode) 
{
    // wait for the device to finish doing it's things
    _renderDevice->waitForDevice();

    // destroy the swapchain
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

    const vk::SwapchainCreateInfoKHR createInfo
    {
        {},                                         // flags 
        _renderDevice->getSurface(),                // surface
        _imageCount,                                // minImageCount
        _surfaceFormat.format,                      // imageFormat
        _surfaceFormat.colorSpace,                  // imageColorSpace
        vk::Extent2D{ _extent.width, _extent.height },                                          // imageExtent 
        1,                                          // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,   // imageUsage
        _renderDevice->areQueuesSame() ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,         // imageSharingMode 
        queueFamilyIndices,                         // queueFamilyIndices
        vk::SurfaceTransformFlagBitsKHR::eIdentity, // preTransform 
        vk::CompositeAlphaFlagBitsKHR::eOpaque,     // compositeAlpha
        _presentMode,                               // presentMode
        VK_TRUE,                                    // clipped
        VK_NULL_HANDLE                              // oldSwapchain
    };

    _swapChain = _renderDevice->getDevice().createSwapchainKHRUnique(createInfo);

    obtainImages();
    createImageViews();
}

