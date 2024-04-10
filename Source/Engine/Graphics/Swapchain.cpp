#include "Core/Print.h"
#include "RenderWindow.h"
#include "Swapchain.h"

namespace bl {

GfxSwapchain::GfxSwapchain(
    std::shared_ptr<GfxDevice>          device,
    std::shared_ptr<RenderWindow>       window,
    std::optional<VkPresentModeKHR>     presentMode,
    std::optional<VkSurfaceFormatKHR>   surfaceFormat)
    : _device(device)
    , _physicalDevice(device->getPhysicalDevice())
    , _window(window)
    , _imageCount(0)
    , _surfaceFormat(surfaceFormat.value_or(DEFAULT_SURFACE_FORMAT))
    , _presentMode(presentMode.value_or(VK_PRESENT_MODE_FIFO_KHR))
    , _swapchain(VK_NULL_HANDLE)
{
    ensureSurfaceSupported();
    recreate(_presentMode);
}

GfxSwapchain::~GfxSwapchain()
{
    destroyImageViews();
    vkDestroySwapchainKHR(_device->get(), _swapchain, nullptr);
}

VkSwapchainKHR GfxSwapchain::get() const { return _swapchain; }
VkFormat GfxSwapchain::getFormat() const { return _surfaceFormat.format; }
VkExtent2D GfxSwapchain::getExtent() const { return _extent; }
uint32_t GfxSwapchain::getImageCount() const { return _imageCount; }
VkPresentModeKHR GfxSwapchain::getPresentMode() const { return _presentMode; }
VkSurfaceFormatKHR GfxSwapchain::getSurfaceFormat() const { return _surfaceFormat; }
std::vector<VkImageView> GfxSwapchain::getImageViews() const { return _swapImageViews; }
std::vector<VkImage> GfxSwapchain::getImages() const { return _swapImages; }

void GfxSwapchain::acquireNext(VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex, bool& wasRecreated)
{
    imageIndex = 0;
    wasRecreated = false;

    VkResult result = vkAcquireNextImageKHR(_device->get(), _swapchain, UINT32_MAX, semaphore, fence, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        wasRecreated = true;
        recreate();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Could not acquire the next swapchain image!");
    }
}

void GfxSwapchain::ensureSurfaceSupported()
{
    VkBool32 supported = false;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice->get(), _device->getPresentFamilyIndex(), _window->getSurface(), &supported))

    if (!supported) {
        throw std::runtime_error("Could not ensure Vulkan surface support on physical device!");
    }
}

void GfxSwapchain::chooseImageCount()
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice->get(), _window->getSurface(), &capabilities))

    const uint32_t DESIRED_IMAGE_COUNT = 3;

    // When it's zero the GPU really doesn't care.
    if (capabilities.maxImageCount == 0) {
        if (capabilities.minImageCount > DESIRED_IMAGE_COUNT) _imageCount = capabilities.minImageCount;
        else _imageCount = DESIRED_IMAGE_COUNT;
    } else if (capabilities.minImageCount >= DESIRED_IMAGE_COUNT && capabilities.maxImageCount <= DESIRED_IMAGE_COUNT) {
        _imageCount = DESIRED_IMAGE_COUNT; // we got this.
    } else {
        _imageCount = capabilities.minImageCount;
    }
}

void GfxSwapchain::chooseFormat(std::optional<VkSurfaceFormatKHR> format)
{

    // Get the surface formats from the physical device.
    std::vector<VkSurfaceFormatKHR> formats = _physicalDevice->getSurfaceFormats(_window);

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
    blWarning("Surface format not found, using default.");
}

void GfxSwapchain::choosePresentMode(std::optional<VkPresentModeKHR> presentMode)
{
    // Obtain the present modes from the physical device.
    std::vector<VkPresentModeKHR> modes = _physicalDevice->getPresentModes(_window);

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

void GfxSwapchain::chooseExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = {};

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice->get(), _window->getSurface(), &capabilities))

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        _extent = capabilities.currentExtent;
    } else {
        glm::ivec2 extent = _window->getExtent();
        _extent = VkExtent2D{ 
            std::clamp((uint32_t)extent.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp((uint32_t)extent.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void GfxSwapchain::obtainImages()
{
    VK_CHECK(vkGetSwapchainImagesKHR(_device->get(), _swapchain, &_imageCount, nullptr))
    _swapImages.resize(_imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(_device->get(), _swapchain, &_imageCount, _swapImages.data()))
}

void GfxSwapchain::createImageViews()
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

        VK_CHECK(vkCreateImageView(_device->get(), &imageViewInfo, nullptr, &_swapImageViews[i]))
    }
}

void GfxSwapchain::destroyImageViews()
{
    for (uint32_t i = 0; i < _swapImageViews.size(); i++) {
        if (_swapImageViews[i] != VK_NULL_HANDLE) vkDestroyImageView(_device->get(), _swapImageViews[i], nullptr);
    }

    _swapImageViews.clear();
}

void GfxSwapchain::recreate(std::optional<VkPresentModeKHR> presentMode)
{
    // Wait for the device to finish doing it's things.
    _device->waitForDevice();

    // Destroy the previous swapchain and it's image views.
    destroyImageViews();
    vkDestroySwapchainKHR(_device->get(), _swapchain, nullptr);

    _swapImageViews.clear();
    _swapImages.clear();

    // Begin recreating the swapchain.
    chooseImageCount();
    chooseFormat(_surfaceFormat);
    chooseExtent();
    choosePresentMode(presentMode);

    if (_extent.width == 0 || _extent.height == 0) {
        blWarning("GfxSwapchain recreation requires a window resize greater than 0x0.");
    }

    // Swapchains use both graphics and present, provide it the queue families.
    std::vector queueFamilyIndices = { _device->getGraphicsFamilyIndex(), _device->getPresentFamilyIndex() };

    // If they are the same index then we don't want to misreport.
    queueFamilyIndices.resize(_device->areQueuesSame() ? 1 : 2);

    // do these out of the create info initialization
    // create the swapchain
    auto imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    auto imageSharingMode = _device->areQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_EXCLUSIVE;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = {};
    createInfo.surface = _window->getSurface();
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

    VK_CHECK(vkCreateSwapchainKHR(_device->get(), &createInfo, nullptr, &_swapchain));

    obtainImages();
    createImageViews();
}

} // namespace bl
