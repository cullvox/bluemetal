#include "Core/Print.h"
#include "Device.h"
#include "Window.h"
#include "Swapchain.h"

namespace bl {

Swapchain::Swapchain(
    Device* device,
    Window* window)
    : _device(device)
    , _physicalDevice(_device->GetPhysicalDevice())
    , _window(window)
    , _imageCount(0)
    , _swapchain(VK_NULL_HANDLE) {
    EnsureSurfaceSupported();
    Recreate();
}

Swapchain::~Swapchain() {
    DestroyImageViews();
    vkDestroySwapchainKHR(_device->Get(), _swapchain, nullptr);
}

VkSwapchainKHR Swapchain::Get() const { 
    return _swapchain; 
}

VkFormat Swapchain::GetFormat() const {
     return _surfaceFormat.format;
}

VkExtent2D Swapchain::GetExtent() const {
     return _extent; 
}

uint32_t Swapchain::GetImageCount() const {
     return _imageCount; 
}

VkPresentModeKHR Swapchain::GetPresentMode() const {
     return _presentMode; 
}

VkSurfaceFormatKHR Swapchain::GetSurfaceFormat() const {
     return _surfaceFormat; 
}

std::vector<VkImage> Swapchain::GetImages() const {
    return _swapImages;
}

std::vector<VkImageView> Swapchain::GetImageViews() const {
     return _swapImageViews; 
}

uint32_t Swapchain::GetImageIndex() const {
    return _imageIndex;
}

bool Swapchain::GetMailboxSupported() const {
    return _isMailboxSupported;
}

bool Swapchain::GetImmediateSupported() const {
    return _isImmediateSupported;
}

bool Swapchain::AcquireNext(VkSemaphore semaphore, VkFence fence) {
    VkResult result = vkAcquireNextImageKHR(_device->Get(), _swapchain, UINT32_MAX, semaphore, fence, &_imageIndex);
    
    switch (result) {
    case VK_SUBOPTIMAL_KHR:
    case VK_SUCCESS: 
        break; // The swapchain does not need to be recreated.
    case VK_ERROR_OUT_OF_DATE_KHR:
        Recreate();
        break;
    default:
        throw std::runtime_error("Could not acquire the next swapchain image!");
    }

    return _wasRecreated;
}

bool Swapchain::QueuePresent(VkSemaphore signalSemaphore) {
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.pImageIndices = &_imageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(_device->GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
       Recreate(); // Next frame _wasRecreated will be reset.
       return true;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Could not queue Vulkan present!");
    }

    _wasRecreated = false; // Reset the recreation field.
    return false;
}

void Swapchain::EnsureSurfaceSupported() {
    VkBool32 supported = false;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice->Get(), _device->GetPresentFamilyIndex(), _window->GetSurface(), &supported))

    if (!supported) {
        throw std::runtime_error("Could not ensure Vulkan surface support on physical device!");
    }
}

void Swapchain::ChooseImageCount() {
    VkSurfaceCapabilitiesKHR capabilities = {};
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice->Get(), _window->GetSurface(), &capabilities))

    // When it's zero the GPU really doesn't care.
    if (capabilities.maxImageCount == 0) {
        if (capabilities.minImageCount > GraphicsConfig::desiredImageCount) _imageCount = capabilities.minImageCount;
        else _imageCount = GraphicsConfig::desiredImageCount;
    } else if (capabilities.minImageCount >= GraphicsConfig::desiredImageCount && capabilities.maxImageCount <= GraphicsConfig::desiredImageCount) {
        _imageCount = GraphicsConfig::desiredImageCount; // we got this.
    } else {
        _imageCount = capabilities.minImageCount;
    }
}

void Swapchain::ChooseFormat() {
    // Get the surface formats from the physical device.
    std::vector<VkSurfaceFormatKHR> formats = _physicalDevice->GetSurfaceFormats(_window);

    // Look for the desired surface format.
    if (std::any_of(formats.begin(), formats.end(), [](auto sf) {
            auto dsf = GraphicsConfig::defaultSurfaceFormat;
            return sf.format == dsf.format && sf.colorSpace == dsf.colorSpace;
        })) 
    {
        _surfaceFormat = GraphicsConfig::defaultSurfaceFormat;
    } 
    else // As fallback use the first format available.
    {    
        blWarning("Surface format not found, using default.");
        _surfaceFormat = formats.front();
    }
}

void Swapchain::ChoosePresentMode()
{
    // Obtain the present modes from the physical device.
    std::vector<VkPresentModeKHR> modes = _physicalDevice->GetPresentModes(_window);

    // Find the quick supported checks.
    _isMailboxSupported = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end();
    _isImmediateSupported = std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != modes.end();

    // Try to find the caller's present mode or use the default instead.
    if (std::find(modes.begin(), modes.end(), GraphicsConfig::defaultPresentMode) != modes.end()) {
        _presentMode = GraphicsConfig::defaultPresentMode;
        return;
    }

    // If our mode wasn't found just use FIFO, as it's reliable and always available.
    blWarning("Present mode {} is unavailable, using VK_PRESENT_MODE_FIFO_KHR.", string_VkPresentModeKHR(GraphicsConfig::defaultPresentMode));
    _presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void Swapchain::ChooseExtent() {
    VkSurfaceCapabilitiesKHR capabilities = {};

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice->Get(), _window->GetSurface(), &capabilities))

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        _extent = capabilities.currentExtent;
    } else {
        VkExtent2D extent = _window->GetExtent();
        _extent = { 
            std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }
}

void Swapchain::ObtainImages()
{
    VK_CHECK(vkGetSwapchainImagesKHR(_device->Get(), _swapchain, &_imageCount, nullptr))
    _swapImages.resize(_imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(_device->Get(), _swapchain, &_imageCount, _swapImages.data()))
}

void Swapchain::CreateImageViews() {
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

        VK_CHECK(vkCreateImageView(_device->Get(), &imageViewInfo, nullptr, &_swapImageViews[i]))
    }
}

void Swapchain::DestroyImageViews() {
    for (VkImageView iv : _swapImageViews)
        vkDestroyImageView(_device->Get(), iv, nullptr);

    _swapImageViews.clear();
}

void Swapchain::Recreate(std::optional<VkPresentModeKHR> presentMode, std::optional<VkSurfaceFormatKHR> surfaceFormat) {
    _device->WaitForDevice(); // Since recreating the swapchain is a big operation, just wait for any processes to sync.

    DestroyImageViews(); // Images will be recreated later.
    ChooseImageCount();
    ChooseExtent();

    if (_extent.width == 0 || _extent.height == 0) {
        throw std::runtime_error("Cannot create a swapchain at an invalid extent!");
    }
    
    if (!presentMode.has_value())
        ChoosePresentMode();

    if (!surfaceFormat.has_value())
        ChooseFormat();

    // The swapchain will use both graphics and present, provide it the proper queue families.
    std::vector queueFamilyIndices = { _device->GetGraphicsFamilyIndex(), _device->GetPresentFamilyIndex() };

    // If they are the same index then we don't want to misreport.
    queueFamilyIndices.resize(_device->GetAreQueuesSame() ? 1 : 2);

    auto imageSharingMode = _device->GetAreQueuesSame() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    auto format = surfaceFormat.value_or(_surfaceFormat);
    auto present = presentMode.value_or(_presentMode); 
    auto oldSwapchain = _swapchain;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = _window->GetSurface();
    createInfo.minImageCount = _imageCount;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageExtent = _extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageSharingMode = imageSharingMode;
    createInfo.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    VK_CHECK(vkCreateSwapchainKHR(_device->Get(), &createInfo, nullptr, &_swapchain));

    if (oldSwapchain) {
        vkDestroySwapchainKHR(_device->Get(), oldSwapchain, nullptr);
    }

    _surfaceFormat = format;
    _presentMode = present; 

    ObtainImages();
    CreateImageViews();

    _wasRecreated = true;
}

bool Swapchain::WasRecreated() const {
    return _wasRecreated;
}

} // namespace bl
