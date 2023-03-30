
#include "Core/Version.hpp"
#include "Core/Log.hpp"
#include "Core/Precompiled.hpp"
#include "Render/RenderDevice.hpp"

#define VMA_IMPLEMENTATION
#include "Render/Vulkan/vk_mem_alloc.h"

namespace bl 
{

RenderDevice::RenderDevice() noexcept
    : m_pWindow(nullptr)
    , m_instance(VK_NULL_HANDLE)
    , m_physicalDevice(VK_NULL_HANDLE)
    , m_graphicsFamilyIndex(0), m_presentFamilyIndex(0)
    , m_device(VK_NULL_HANDLE)
    , m_graphicsQueue(VK_NULL_HANDLE)
    , m_presentQueue(VK_NULL_HANDLE)
    , m_commandPool(VK_NULL_HANDLE)
    , m_allocator(VK_NULL_HANDLE)
{
}

RenderDevice::RenderDevice(Window& window) noexcept
    : RenderDevice()
{
    m_pWindow = &window;

    if (not createInstance() ||
        not choosePhysicalDevice() ||
        not createDevice() ||
        not createCommandPool() ||
        not createAllocator())
    {
        Logger::Error("Could not create the render device!\n");
        collapse();
    }
}

RenderDevice::~RenderDevice()
{
    collapse();
}

RenderDevice& RenderDevice::operator=(RenderDevice&& rhs) noexcept
{
    collapse();

    m_pWindow = rhs.m_pWindow;
    m_instance = rhs.m_instance;
    m_physicalDevice = rhs.m_physicalDevice;
    m_graphicsFamilyIndex = rhs.m_graphicsFamilyIndex;
    m_presentFamilyIndex = rhs.m_presentFamilyIndex;
    m_device = rhs.m_device;
    m_graphicsQueue = rhs.m_graphicsQueue;
    m_presentQueue = rhs.m_presentQueue;
    m_commandPool = rhs.m_commandPool;
    m_allocator = rhs.m_allocator;

    rhs.m_pWindow = nullptr;
    rhs.m_instance = VK_NULL_HANDLE;
    rhs.m_physicalDevice = VK_NULL_HANDLE;
    rhs.m_graphicsFamilyIndex = 0;
    rhs.m_presentFamilyIndex = 0;
    rhs.m_device = VK_NULL_HANDLE;
    rhs.m_graphicsQueue = VK_NULL_HANDLE;
    rhs.m_presentQueue = VK_NULL_HANDLE;
    rhs.m_commandPool = VK_NULL_HANDLE;
    rhs.m_allocator = VK_NULL_HANDLE;

    return *this;
}

VkInstance RenderDevice::getInstance() const noexcept
{
    return m_instance;
}

VkPhysicalDevice RenderDevice::getPhysicalDevice() const noexcept
{
    return m_physicalDevice;
}

uint32_t RenderDevice::getGraphicsFamilyIndex() const noexcept
{
    return m_graphicsFamilyIndex;
}

uint32_t RenderDevice::getPresentFamilyIndex() const noexcept
{
    return m_presentFamilyIndex;
}

VkDevice RenderDevice::getDevice() const noexcept
{
    return m_device;
}

VkQueue RenderDevice::getGraphicsQueue() const noexcept
{
    return m_graphicsQueue;
}

VkQueue RenderDevice::getPresentQueue() const noexcept
{
    return m_presentQueue;
}

VkCommandPool RenderDevice::getCommandPool() const noexcept
{
    return m_commandPool;
}

VmaAllocator RenderDevice::getAllocator() const noexcept
{
    return m_allocator;
}

bool RenderDevice::areQueuesSame() const noexcept
{
    return m_graphicsFamilyIndex == m_presentFamilyIndex;
}

VkFormat RenderDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const noexcept
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties = {};
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    Logger::Error("Could not find any valid format!\n");

    return VK_FORMAT_UNDEFINED;
}

bool RenderDevice::immediateSubmit(std::function<void(VkCommandBuffer)> recorder)
{

    const VkCommandBufferAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer temporaryCommandBuffer{};
    if (vkAllocateCommandBuffers(m_device, &allocateInfo, &temporaryCommandBuffer) != VK_SUCCESS)
    {
        Logger::Error("Could not allocate a temporary command buffer for immediate submit!");
        return false;
    }

    auto freeCmdBuffers = [&]() { vkFreeCommandBuffers(m_device, m_commandPool, 1, &temporaryCommandBuffer); };

    const VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };

    if (vkBeginCommandBuffer(temporaryCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        Logger::Error("Could not begin a temporary command buffer for immediate submit!");
        freeCmdBuffers();
        return false;
    }

    recorder(temporaryCommandBuffer);

    if (vkEndCommandBuffer(temporaryCommandBuffer) != VK_SUCCESS)
    {
        Logger::Error("Could not end a temporary command buffer for immediate submit!");
        freeCmdBuffers();
        return false;
    }

    const VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &temporaryCommandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr
    };

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        Logger::Error("Could not submit a temporary command buffer for immediate submit!");
        freeCmdBuffers();
        return false;
    }

    vkQueueWaitIdle(m_graphicsQueue);

    freeCmdBuffers();
    return true;
}

bool RenderDevice::good() const noexcept
{
    return m_pWindow != nullptr
        && m_instance != VK_NULL_HANDLE
        && m_device != VK_NULL_HANDLE
        && m_commandPool != VK_NULL_HANDLE
        && m_allocator != VK_NULL_HANDLE;
}

void RenderDevice::waitForDevice() const noexcept
{
    vkDeviceWaitIdle(m_device);
}

bool RenderDevice::getValidationLayers(std::vector<const char*>& layers) const noexcept
{
    static const std::vector<const char*> requiredLayers = {
        "VK_LAYER_KHRONOS_validation",  
    };

    // Get the count of validation layers on the system.
    uint32_t layerPropertiesCount = 0;
    if (vkEnumerateInstanceLayerProperties(&layerPropertiesCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate validation layers!\n");
        return false;
    }

    // Allocate an array the proper size to hold all the layer properties.
    std::vector<VkLayerProperties> layerProperties;
    layerProperties.resize(layerPropertiesCount);

    // Fill the layer properties array.
    if (vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate validation layers!\n");
        return false;
    }

    // Iterate through the required layers to find any that the system does not support.
    for (const char* name : requiredLayers)
    {
        const auto foundLayer = 
            std::find_if(
                layerProperties.begin(), 
                layerProperties.end(), 
            [name](const VkLayerProperties& properties){ 
                return std::strcmp(properties.layerName, name) == 0; 
            });

        if (foundLayer == layerProperties.end())
        {
            Logger::Error("Could not find required validation layer: {}\n", name);
            return false;
        }
    }

    layers = requiredLayers;
    return true;
}

bool RenderDevice::getInstanceExtensions(std::vector<const char*>& extensions) const noexcept
{

    // Get our engines required extensions.
    std::vector<const char*> requiredExtensions = {
#ifndef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
    };

    // The surface extensions are required too.
    std::vector<const char*> surfaceExtensions{}; 
    if (not m_pWindow->getVulkanInstanceExtensions(surfaceExtensions))
    {
        Logger::Error("Could not get the vulkan instance extensions!\n");
        return false;
    }
    
    requiredExtensions.insert(requiredExtensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // Get all the current vulkan instance extensions.
    uint32_t extensionPropertiesCount = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate instance extension properties!\n");
        return false;
    }

    // Allocate an array to hold the extension properties.
    std::vector<VkExtensionProperties> extensionProperties{};
    extensionProperties.resize(extensionPropertiesCount);

    // Fill the array of extension properties.
    if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, extensionProperties.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate instance extension properties!\n");
        return false;
    }

    // Check and make sure all of our extensions are present.
    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
        [pName](const VkExtensionProperties& properties){
                return std::strcmp(pName, properties.extensionName) == 0; }) == extensionProperties.end())
        {
            Logger::Error("Could not find required instance extension: {}\n", pName);
            return false;
        }
    }

    // We have all our extensions!
    extensions = requiredExtensions;
    return true;
}

bool RenderDevice::createInstance() noexcept
{
    std::vector<const char*> instanceExtensions{};
    std::vector<const char*> validationLayers{};

    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |  
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,    
    };

    const VkApplicationInfo applicationInfo{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = applicationName.data(),
        .applicationVersion = VK_MAKE_VERSION(applicationVersionMajor, applicationVersionMinor, applicationVersionPatch),
        .pEngineName        = engineName.data(),
        .engineVersion      = VK_MAKE_VERSION(engineVersionMajor, engineVersionMinor, engineVersionPatch),
        .apiVersion         = VK_API_VERSION_1_2,
    };

    if (not getInstanceExtensions(instanceExtensions))
    {
        Logger::Error("Could not retrieve the instance extensions!\n");
        return false;
    }
    
    if (not getValidationLayers(validationLayers))
    {
        Logger::Debug("Could not retrieve the validation layers, they will not be enabled!\n");
    }

    const VkInstanceCreateInfo createInfo{
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                      = &debugMessengerCreateInfo,
        .flags                      = 0,
        .pApplicationInfo           = &applicationInfo,
        .enabledLayerCount          = (uint32_t)validationLayers.size(),
        .ppEnabledLayerNames        = validationLayers.data(),
        .enabledExtensionCount      = (uint32_t)instanceExtensions.size(),
        .ppEnabledExtensionNames    = instanceExtensions.data(),
    };

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        Logger::Error("Could not create a vulkan instance!\n");
        return false;
    }
}

bool RenderDevice::choosePhysicalDevice() noexcept
{
    uint32_t                        physicalDevicesCount;
    std::vector<VkPhysicalDevice>   physicalDevices{};
    VkPhysicalDeviceProperties      physicalDeviceProperties{};

    if (vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate physical devices!\n");
        return false;
    }
    
    physicalDevices.resize(physicalDevicesCount);
    
    if (vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, physicalDevices.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate physical devices!\n");
        return false;
    }

    // TODO: Add a parameter hint for selecting physical devices.
    m_physicalDevice = physicalDevices[0];

    vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);
    Logger::Debug("Using vulkan physical device: {}\n", physicalDeviceProperties.deviceName);
    
    return true;
}

bool RenderDevice::getDeviceExtensions(std::vector<const char*>& extensions) const noexcept
{
    std::vector<const char*>            requiredExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    uint32_t                            deviceExtensionsCount = 0;
    std::vector<VkExtensionProperties>  extensionProperties{};

    extensions.clear();

    if (vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionsCount, nullptr) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate vulkan device extensions!\n");
        return false;
    }

    extensionProperties.resize(deviceExtensionsCount);
    
    if (vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionsCount, extensionProperties.data()) != VK_SUCCESS)
    {
        Logger::Error("Could not enumerate vulkan device extensions!\n");
        return false;
    }

    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
        [pName](const VkExtensionProperties& properties){
                return std::strcmp(pName, properties.extensionName) == 0; }) == extensionProperties.end())
        {
            Logger::Error("Could not find required instance extension: {}\n", pName);
            return false;
        }
    }

    extensions = requiredExtensions;
    return true;
}

bool RenderDevice::createDevice() noexcept
{
    uint32_t                                queueFamilyPropertiesCount = 0;
    std::vector<VkQueueFamilyProperties>    queueFamilyProperties{};
    std::vector<const char*>                deviceExtensions{};
    std::vector<const char*>                validationLayers{};

    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertiesCount, nullptr);
    queueFamilyProperties.resize(queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

    // Create a temporary surface for device creation.
    VkSurfaceKHR temporarySurface;
    if (not m_pWindow->createVulkanSurface(m_instance, temporarySurface))
    {
        Logger::Error("Could not create the temporary vulkan surface for device creation!\n");
        return false;
    }

    // Determine the queues for graphics and present.
    uint32_t queueFamilyIndex = 0;
    VkBool32 presentSupport = false;
    for (const VkQueueFamilyProperties& properties : queueFamilyProperties)
    {

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsFamilyIndex = queueFamilyIndex; 
        }

        bool success = vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, queueFamilyIndex, temporarySurface, &presentSupport) == VK_SUCCESS;
        if (success && presentSupport)
        {
            m_presentFamilyIndex = queueFamilyIndex;
        }

        queueFamilyIndex++;
    }

    vkDestroySurfaceKHR(m_instance, temporarySurface, nullptr);

    const float queuePriorities[] = { 1.0f, 1.0f };
    const std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_graphicsFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_presentFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        }
    };

    if (not getDeviceExtensions(deviceExtensions))
    {
        Logger::Error("Could not get the vulkan device extensions!\n");
        return false;
    }

    if (not getValidationLayers(validationLayers))
    {
        Logger::Debug("Could not retrieve the validation layers, they will not be enabled!\n");
    }

    const VkPhysicalDeviceFeatures features{};
    const VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = areQueuesSame() ? (uint32_t)1 : (uint32_t)2,
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = (uint32_t)validationLayers.size(),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = (uint32_t)deviceExtensions.size(),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &features,
    };

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        Logger::Error("Could not create a vulkan logical device!\n");
        return false;
    }

    vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_presentFamilyIndex, 0, &m_presentQueue);
    
    return true;
}

bool RenderDevice::createCommandPool() noexcept
{
    const VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_graphicsFamilyIndex
    };

    if (vkCreateCommandPool(m_device, &createInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        Logger::Error("Could not create the command pool!\n");
        return false;
    }

    return true;
}

bool RenderDevice::createAllocator() noexcept
{
    const VmaAllocatorCreateInfo createInfo{
        .flags = 0,
        .physicalDevice = m_physicalDevice,
        .device = m_device,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = nullptr,
        .instance = m_instance,
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    if (vmaCreateAllocator(&createInfo, &m_allocator) != VK_SUCCESS)
    {
        Logger::Error("Could not create the vulkan memory allocator!\n");
        return false;
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL RenderDevice::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept
{
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        Logger::Error("{}", pCallbackData->pMessage);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        Logger::Debug("{}", pCallbackData->pMessage);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        Logger::Log("{}", pCallbackData->pMessage);
    }

    return false;
}

void RenderDevice::collapse() noexcept
{
    if (m_allocator) vmaDestroyAllocator(m_allocator);
    if (m_commandPool) vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    if (m_device) vkDestroyDevice(m_device, nullptr);
    if (m_instance) vkDestroyInstance(m_instance, nullptr);

    m_pWindow = nullptr;
    m_allocator = VK_NULL_HANDLE;
    m_commandPool = VK_NULL_HANDLE;
    m_presentQueue = VK_NULL_HANDLE;
    m_graphicsQueue = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
    m_presentFamilyIndex = 0;
    m_graphicsFamilyIndex = 0;
    m_physicalDevice = VK_NULL_HANDLE;
    m_instance = VK_NULL_HANDLE;
}

} /* namespace bl */