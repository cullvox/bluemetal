#include "Swapchain.h"
#include "Core/Log.h"

namespace bl {

Swapchain::Swapchain(GraphicsDevice* pDevice, Window* pWindow,
    std::optional<VkPresentModeKHR> presentMode, std::optional<VkSurfaceFormatKHR> surfaceFormat)
    : _pPhysicalDevice(pDevice->getPhysicalDevice())
    , _pDevice(pDevice)
    , _pWindow(pWindow)
    , _imageCount(0)
    , _surfaceFormat(surfaceFormat.value_or(DEFAULT_SURFACE_FORMAT))
    , _presentMode(VK_PRESENT_MODE_FIFO_KHR)
    , _extent({})
    , _swapchain(VK_NULL_HANDLE)
    , _swapImages({})
    , _swapImageViews({})
    , _isMailboxSupported(false)
    , _isImmediateSupported(false)
{
    ensureSurfaceSupported();
    recreate(presentMode);
}

Swapchain::~Swapchain()
{
    destroyImageViews();
    vkDestroySwapchainKHR(_pDevice->getHandle(), _swapchain, nullptr);
}

VkSwapchainKHR Swapchain::getHandle() { return _swapchain; }

VkFormat Swapchain::getFormat() { return _surfaceFormat.format; }

VkExtent2D Swapchain::getExtent() { return _extent; }

uint32_t Swapchain::getImageCount() { return _imageCount; }

VkPresentModeKHR Swapchain::getPresentMode() { return _presentMode; }

VkSurfaceFormatKHR Swapchain::getSurfaceFormat() { return _surfaceFormat; }

std::vector<VkImageView> Swapchain::getImageViews() { return _swapImageViews; }

std::vector<VkImage> Swapchain::getImages() { return _swapImages; }

void Swapchain::acquireNext(
    VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex, bool* pRecreated)
{
    *pImageIndex = 0;
    *pRecreated = false;

    VkResult result = vkAcquireNextImageKHR(
        _pDevice->getHandle(), _swapchain, UINT32_MAX, semaphore, fence, pImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate();
        *pRecreated = true;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Could not acquire the next swapchain image!");
    }
}

void Swapchain::ensureSurfaceSupported()
{
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_pPhysicalDevice->getHandle(),
        _pDevice->getPresentFamilyIndex(), _pWindow->getSurface(), &supported);

    if (!supported) {
        throw std::runtime_error("Could not ensure Vulkan surface support on physical device!");
    }
}

void Swapchain::chooseImageCount()
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            _pPhysicalDevice->getHandle(), _pWindow->getSurface(), &capabilities)
        != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan surface capabilities!");
    }

    // Try to at least get one over the minimum
    _imageCount = (capabilities.minImageCount + 1 <= capabilities.maxImageCount)
        ? capabilities.minImageCount + 1
        : capabilities.maxImageCount;
}

void Swapchain::chooseFormat(std::optional<VkSurfaceFormatKHR> format)
{

    // Get the surface formats from the physical device.
    std::vector<VkSurfaceFormatKHR> formats = _pPhysicalDevice->getSurfaceFormats(_pWindow);

    // find the surface format/colorspace to be used
    VkSurfaceFormatKHR toFind = format.value_or(DEFAULT_SURFACE_FORMAT);

    auto pred = [&toFind](const VkSurfaceFormatKHR& sf) {
        return sf.format == toFind.format && sf.colorSpace == toFind.colorSpace;
    };

    auto it = std::find_if(formats.begin(), formats.end(), pred);

    if (it != formats.end()) {
        _surfaceFormat = toFind;
        return;
    }

    // as fallback use the first format
    _surfaceFormat = formats.front();
    BL_LOG(LogType::eWarning,
        "Could not find Vulkan default surface format: ({}, {}), falling back to: ({}, {}).",
        string_VkFormat(toFind.format), string_VkColorSpaceKHR(toFind.colorSpace),
        string_VkFormat(_surfaceFormat.format),
        string_VkColorSpaceKHR(_surfaceFormat.colorSpace));
}

bool Swapchain::choosePresentMode(std::optional<VkPresentModeKHR> presentMode) noexcept
{
    // Obtain the present modes from the physical device.
    std::vector<VkPresentModeKHR> modes = _pPhysicalDevice->getPresentModes(_pWindow);

    // Find the quick supported checks.
    _isMailboxSupported = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end();
    _isImmediateSupported = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != modes.end();

    // Try to find the caller's present mode or use the default instead.
    auto toFind = presentMode.value_or(DEFAULT_PRESENT_MODE);
    auto it = std::find(modes.begin(), modes.end(), toFind);

    if (it != modes.end()) {
        _presentMode = toFind;
        return;
    }

    blWarning("Present mode {} is unavailable, using VK_PRESENT_MODE_FIFO_KHR.", string_VkPresentModeKHR(DEFAULT_PRESENT_MODE));

    // If our mode wasn't found just use FIFO, as that's always available.
    _presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

bool Swapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = {};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_pPhysicalDevice->getHandle(), _pWindow->getSurface(), &capabilities) != VK_SUCCESS) {
        blError("Could not get Vulkan physical device surface capabilities!");
        return false;
    }

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        _extent = capabilities.currentExtent;
    } else {
        Extent2D extent = _pWindow->getExtent();
        _extent = VkExtent2D{ 
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    return true;
}

bool Swapchain::obtainImages()
{
    uint32_t imageCount = 0;
    if (vkGetSwapchainImagesKHR(_pDevice->getHandle(), _swapchain, &imageCount, nullptr) != VK_SUCCESS) {
        blError("Could not get Vulkan swapchain image count!");
        return false;
    }

    _swapImages.resize(imageCount);

    if (vkGetSwapchainImagesKHR(_pDevice->getHandle(), _swapchain, &imageCount, _swapImages.data()) != VK_SUCCESS) {
        blError("Could not get VkSwapchainKHR images!");
        return false;
    }

    return true;
}

bool Swapchain::createImageViews()
{
    _swapImageViews.resize(_imageCount);

    for (uint32_t i = 0; i < _imageCount; i++) {
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
        imageViewInfo.image = _swapImages[i];
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = _surfaceFormat.format;
        imageViewInfo.components = componentMapping;
        imageViewInfo.subresourceRange = subresourceRange;

        if (vkCreateImageView(_pDevice->getHandle(), &imageViewInfo, nullptr, &_swapImageViews[i]) != VK_SUCCESS) {
            blError("Could not create a Vulkan image view for the swapchain!");
            destroyImageViews();
            return false;
        }
    }

    return true;
}

void Swapchain::destroyImageViews()
{
    for (uint32_t i = 0; i < _swapImageViews.size(); i++) {
        if (_swapImageViews[i] != VK_NULL_HANDLE)
            vkDestroyImageView(_pDevice->getHandle(), _swapImageViews[i], nullptr);
    }

    _swapImageViews.clear();
}

void Swapchain::recreate(std::optional<VkPresentModeKHR> presentMode)
{
    // Wait for the device to finish doing it's things.
    _pDevice->waitForDevice();

    // Destroy the previous swapchain and it's image views.
    vkDestroySwapchainKHR(_pDevice->getHandle(), _swapchain, nullptr);
    destroyImageViews();

    // Begin recreating the swapchain.
    chooseImageCount();
    chooseFormat(_surfaceFormat);
    chooseExtent();
    choosePresentMode(presentMode);

    if (_extent.width == 0 || _extent.height == 0) {
        blInfo("Swapchain recreation requires a window resize greater than 0x0.");
        return;
    }

    // Swapchains use both graphics and present, provide it the queue families.
    std::vector queueFamilyIndices = { _pDevice->getGraphicsFamilyIndex(), _pDevice->getPresentFamilyIndex() };

    // If they are the same index then we don't want to misreport.
    queueFamilyIndices.resize(_pDevice->areQueuesSame() ? 1 : 2);

    // do these out of the create info initialization
    // create the swapchain
    auto imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    auto imageSharingMode = _pDevice->areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_EXCLUSIVE;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = {};
    createInfo.surface = _pWindow->getSurface();
    createInfo.minImageCount = _imageCount;
    createInfo.imageFormat = _surfaceFormat.format;
    createInfo.imageColorSpace = _surfaceFormat.colorSpace;
    createInfo.imageExtent = (VkExtent2D)_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = imageUsage;
    createInfo.imageSharingMode = imageSharingMode;
    createInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = _presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_pDevice->getHandle(), &createInfo, nullptr, &_swapchain) != VK_SUCCESS) {
        blError("Could not create a VkSwapchain!");
        return false;
    }

    obtainImages();
    createImageViews();
}

void Swapchain::recreate() { recreate(_presentMode); }

} // namespace bl
