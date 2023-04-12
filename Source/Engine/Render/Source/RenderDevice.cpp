
#include "Core/Version.hpp"
#include "Core/Log.hpp"
#include "Core/Precompiled.hpp"
#include "Render/RenderDevice.hpp"

#define VMA_IMPLEMENTATION
#include "Render/Vulkan/vk_mem_alloc.h"


blRenderDevice::blRenderDevice(std::shared_ptr<blWindow> window)
{
    _window = window;
    
    createInstance();
    choosePhysicalDevice();
    createDevice();
    createCommandPool();
    createAllocator();
}

blRenderDevice::~blRenderDevice()
{
    vmaDestroyAllocator(_allocator);
    vkDestroyCommandPool(_device, _commandPool, nullptr);
    vkDestroyDevice(_device, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

VkInstance blRenderDevice::getInstance() const noexcept
{
    return _instance;
}

VkPhysicalDevice blRenderDevice::getPhysicalDevice() const noexcept
{
    return _physicalDevice;
}

uint32_t blRenderDevice::getGraphicsFamilyIndex() const noexcept
{
    return _graphicsFamilyIndex;
}

uint32_t blRenderDevice::getPresentFamilyIndex() const noexcept
{
    return _presentFamilyIndex;
}

VkDevice blRenderDevice::getDevice() const noexcept
{
    return _device;
}

VkQueue blRenderDevice::getGraphicsQueue() const noexcept
{
    return _graphicsQueue;
}

VkQueue blRenderDevice::getPresentQueue() const noexcept
{
    return _presentQueue;
}

VkCommandPool blRenderDevice::getCommandPool() const noexcept
{
    return _commandPool;
}

VmaAllocator blRenderDevice::getAllocator() const noexcept
{
    return _allocator;
}

bool blRenderDevice::areQueuesSame() const noexcept
{
    return _graphicsFamilyIndex == _presentFamilyIndex;
}

VkFormat blRenderDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties = {};
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Could not find any valid format!");

    return VK_FORMAT_UNDEFINED;
}

void blRenderDevice::immediateSubmit(
    std::function<void(VkCommandBuffer)> recorder)
{

    const VkCommandBufferAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = _commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer temporaryCommandBuffer{};
    if (vkAllocateCommandBuffers(_device, &allocateInfo, &temporaryCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not allocate a temporary command buffer for immediate submit!");
    }

    auto freeCmdBuffers = [&]() { vkFreeCommandBuffers(_device, _commandPool, 1, &temporaryCommandBuffer); };

    const VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };

    if (vkBeginCommandBuffer(temporaryCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        freeCmdBuffers();
        throw std::runtime_error("Could not begin a temporary command buffer for immediate submit!");    
    }

    recorder(temporaryCommandBuffer);

    if (vkEndCommandBuffer(temporaryCommandBuffer) != VK_SUCCESS)
    {
        freeCmdBuffers();
        throw std::runtime_error("Could not end a temporary command buffer for immediate submit!");
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

    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        freeCmdBuffers();
        throw std::runtime_error("Could not submit a temporary command buffer for immediate submit!");
    }

    vkQueueWaitIdle(_graphicsQueue);

    freeCmdBuffers();
}

void blRenderDevice::waitForDevice() const noexcept
{
    vkDeviceWaitIdle(_device);
}

std::vector<const char*> blRenderDevice::getValidationLayers() const
{
    static const std::vector<const char*> requiredLayers = {
        "VK_LAYER_KHRONOS_validation",  
    };

    // Get the count of validation layers on the system.
    uint32_t layerPropertiesCount = 0;
    if (vkEnumerateInstanceLayerProperties(&layerPropertiesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get validation layers count!");
    }

    // Allocate an array the proper size to hold all the layer properties.
    std::vector<VkLayerProperties> layerProperties;
    layerProperties.resize(layerPropertiesCount);

    // Fill the layer properties array.
    if (vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate validation layers!");
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
            throw std::runtime_error(
                "Could not find required validation layer: " + std::string(name));
        }
    }

    return requiredLayers;
}

std::vector<const char*> blRenderDevice::getInstanceExtensions() const
{

    // Get our engines required extensions.
    std::vector<const char*> requiredExtensions = {
#ifndef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
    };

    // The surface extensions are required too.
    std::vector<const char*> surfaceExtensions = _window->getVulkanInstanceExtensions();
    
    requiredExtensions.insert(requiredExtensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // Get all the current vulkan instance extensions.
    uint32_t extensionPropertiesCount = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate instance extension properties!");
    }

    // Allocate an array to hold the extension properties.
    std::vector<VkExtensionProperties> extensionProperties{};
    extensionProperties.resize(extensionPropertiesCount);

    // Fill the array of extension properties.
    if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, extensionProperties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate instance extension properties!");
    }

    // Check and make sure all of our extensions are present.
    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
        [pName](const VkExtensionProperties& properties){
                return std::strcmp(pName, properties.extensionName) == 0; }) == extensionProperties.end())
        {
            throw std::runtime_error("Could not find required instance extension: " + std::string(pName));
        }
    }

    // We have all our extensions!
    return requiredExtensions;
}

void blRenderDevice::createInstance()
{
    std::vector<const char*> instanceExtensions = getInstanceExtensions();
    std::vector<const char*> validationLayers = getValidationLayers();

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

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan instance!");
    }

    BL_LOG(blLogType::eInfo, "Vulkan instance created")
}

void blRenderDevice::choosePhysicalDevice()
{
    uint32_t                        physicalDevicesCount;
    std::vector<VkPhysicalDevice>   physicalDevices{};
    VkPhysicalDeviceProperties      physicalDeviceProperties{};

    if (vkEnumeratePhysicalDevices(_instance, &physicalDevicesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate physical devices!");
    }
    
    physicalDevices.resize(physicalDevicesCount);
    
    if (vkEnumeratePhysicalDevices(_instance, &physicalDevicesCount, physicalDevices.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate physical devices!");
    }

    // TODO: Add a parameter hint for selecting physical devices.
    _physicalDevice = physicalDevices[0];

    vkGetPhysicalDeviceProperties(_physicalDevice, &physicalDeviceProperties);

    BL_LOG(blLogType::eInfo, "Using vulkan physical device: {}", physicalDeviceProperties.deviceName);
}

std::vector<const char*> blRenderDevice::getDeviceExtensions() const
{
    std::vector<const char*>            requiredExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    uint32_t                            deviceExtensionsCount = 0;
    std::vector<VkExtensionProperties>  extensionProperties{};


    if (vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &deviceExtensionsCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate vulkan device extensions!");
    }

    extensionProperties.resize(deviceExtensionsCount);
    
    if (vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &deviceExtensionsCount, extensionProperties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate vulkan device extensions!");
    }

    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
        [pName](const VkExtensionProperties& properties){
                return std::strcmp(pName, properties.extensionName) == 0; }) == extensionProperties.end())
        {
            throw std::runtime_error("Could not find required instance extension: " + std::string(pName));
        }
    }

    return requiredExtensions;
}

void blRenderDevice::createDevice()
{
    uint32_t                                queueFamilyPropertiesCount = 0;
    std::vector<VkQueueFamilyProperties>    queueFamilyProperties{};
    const std::vector<const char*>          deviceExtensions = getDeviceExtensions();
    const std::vector<const char*>          validationLayers = getValidationLayers();

    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyPropertiesCount, nullptr);
    queueFamilyProperties.resize(queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

    VkSurfaceKHR temporarySurface = _window->createVulkanSurface(_instance); // Window owns this surface instead of this function.

    // Determine the queues for graphics and present.
    uint32_t queueFamilyIndex = 0;
    VkBool32 presentSupport = false;
    for (const VkQueueFamilyProperties& properties : queueFamilyProperties)
    {

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            _graphicsFamilyIndex = queueFamilyIndex; 
        }

        bool success = vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, queueFamilyIndex, temporarySurface, &presentSupport) == VK_SUCCESS;
        if (success && presentSupport)
        {
            _presentFamilyIndex = queueFamilyIndex;
        }

        queueFamilyIndex++;
    }

    const float queuePriorities[] = { 1.0f, 1.0f };
    const std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = _graphicsFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = _presentFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        }
    };

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

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan logical device!");
    }

    vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, _presentFamilyIndex, 0, &_presentQueue);
}

void blRenderDevice::createCommandPool()
{
    const VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = _graphicsFamilyIndex
    };

    if (vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the Vulkan command pool!");
    }

}

void blRenderDevice::createAllocator()
{
    const VmaAllocatorCreateInfo createInfo{
        .flags = 0,
        .physicalDevice = _physicalDevice,
        .device = _device,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = nullptr,
        .instance = _instance,
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the vulkan memory allocator!");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL blRenderDevice::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept
{
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        BL_LOG(blLogType::eError, "{}", pCallbackData->pMessage);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        BL_LOG(blLogType::eWarning, "{}", pCallbackData->pMessage);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        BL_LOG(blLogType::eInfo, "{}", pCallbackData->pMessage);
    }

    return false;
}