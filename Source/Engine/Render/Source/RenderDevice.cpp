
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
}

vk::Instance blRenderDevice::getInstance() const noexcept
{
    return _instance.get();
}

vk::PhysicalDevice blRenderDevice::getPhysicalDevice() const noexcept
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

vk::Device blRenderDevice::getDevice() const noexcept
{
    return _device.get();
}

vk::Queue blRenderDevice::getGraphicsQueue() const noexcept
{
    return _graphicsQueue;
}

vk::Queue blRenderDevice::getPresentQueue() const noexcept
{
    return _presentQueue;
}

vk::CommandPool blRenderDevice::getCommandPool() const noexcept
{
    return _commandPool.get();
}

VmaAllocator blRenderDevice::getAllocator() const noexcept
{
    return _allocator;
}

bool blRenderDevice::areQueuesSame() const noexcept
{
    return _graphicsFamilyIndex == _presentFamilyIndex;
}

vk::Format blRenderDevice::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
    for (vk::Format format : candidates)
    {
        vk::FormatProperties properties = _physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Could not find any valid format!");

    return vk::Format::eUndefined;
}

void blRenderDevice::immediateSubmit(
    const std::function<void(vk::CommandBuffer)>& recorder) const
{
    const vk::CommandBufferAllocateInfo allocateInfo{
        getCommandPool(),
        vk::CommandBufferLevel::ePrimary,
        1
    };

    std::vector<vk::CommandBuffer> allocatedCommandBuffers = getDevice().allocateCommandBuffers(allocateInfo);
    vk::CommandBuffer commandBuffer = allocatedCommandBuffers[0];

    const vk::CommandBufferBeginInfo beginInfo{};
    commandBuffer.begin(beginInfo);
    recorder(commandBuffer);
    commandBuffer.end();

    const vk::SubmitInfo submitInfo{
        {},
        {},
        allocatedCommandBuffers,
        {}
    };

    _graphicsQueue.submit(submitInfo);
    _graphicsQueue.waitIdle();
}

void blRenderDevice::waitForDevice() const noexcept
{
    getDevice().waitIdle();
}

std::vector<const char*> blRenderDevice::getValidationLayers() const
{
    static const std::vector<const char*> requiredLayers = {
        "VK_LAYER_KHRONOS_validation",  
    };

    std::vector<vk::LayerProperties> layerProperties = vk::enumerateInstanceLayerProperties();

    // Iterate through the required layers to find any that the system does not support.
    for (const char* name : requiredLayers)
    {
        if (std::find_if(
                layerProperties.begin(), 
                layerProperties.end(), 
                [name](const vk::LayerProperties& properties)
                { 
                    return std::strcmp(properties.layerName, name) == 0; 
                }
            ) == layerProperties.end())
        {
            BL_LOG(blLogType::eFatal, "Could not find required validation layer: {}", name);
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
    
    requiredExtensions.insert(
        requiredExtensions.end(), 
        surfaceExtensions.begin(), 
        surfaceExtensions.end());

    // Get all the current vulkan instance extensions.
    std::vector<vk::ExtensionProperties> extensionProperties = vk::enumerateInstanceExtensionProperties();

    // Check and make sure all of our extensions are present.
    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(
                extensionProperties.begin(), 
                extensionProperties.end(), 
                    [pName](const vk::ExtensionProperties& properties)
                    {
                        return std::strcmp(pName, properties.extensionName) == 0;
                    }
            ) == extensionProperties.end())
        {
            BL_LOG(blLogType::eFatal, "Could not find required instance extension: {}", pName);
        }
    }

    // We have all our extensions!
    return requiredExtensions;
}

void blRenderDevice::createInstance()
{
    std::vector<const char*> instanceExtensions = getInstanceExtensions();
    std::vector<const char*> validationLayers = getValidationLayers();

    const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        {},
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        debugCallback,
        nullptr    
    };

    const vk::ApplicationInfo applicationInfo{
        applicationName.data(),         // pApplicationName
        VK_MAKE_VERSION(
            applicationVersionMajor,
            applicationVersionMinor,
            applicationVersionPatch),   // applicationVersion
        engineName.data(),              // pEngineName
        VK_MAKE_VERSION(
            engineVersionMajor, 
            engineVersionMinor, 
            engineVersionPatch),        // engineVersion
        VK_API_VERSION_1_2,             // apiVersion
    };

    const vk::InstanceCreateInfo createInfo{
        {},                         // flags
        &applicationInfo,           // pApplicationInfo
        validationLayers,           // pEnabledLayerNames
        instanceExtensions,         // pEnabledExtensionNames
        &debugMessengerCreateInfo,   // pNext
    };

    _instance =  vk::createInstanceUnique(createInfo);

    BL_LOG(blLogType::eInfo, "Vulkan instance created")
}

void blRenderDevice::choosePhysicalDevice()
{
    std::vector<vk::PhysicalDevice> physicalDevices = getInstance().enumeratePhysicalDevices();

    // TODO: Add a parameter: hint for selecting physical devices.
    _physicalDevice = physicalDevices[0];

    vk::PhysicalDeviceProperties properties = _physicalDevice.getProperties();

    BL_LOG(blLogType::eInfo, "Using vulkan physical device: {}", properties.deviceName);
}

std::vector<const char*> blRenderDevice::getDeviceExtensions() const
{
    std::vector<const char*> requiredExtensions{
         VK_KHR_SWAPCHAIN_EXTENSION_NAME 
    };

    std::vector<vk::ExtensionProperties> extensionProperties = _physicalDevice.enumerateDeviceExtensionProperties();

    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(
                extensionProperties.begin(), 
                extensionProperties.end(), 
                [pName](const vk::ExtensionProperties& properties)
                {
                    return std::strcmp(pName, properties.extensionName) == 0; 
                }
            ) == extensionProperties.end())
        {
            BL_LOG(blLogType::eFatal, "Could not find required instance extension: {}", pName);
        }
    }

    return requiredExtensions;
}

void blRenderDevice::createDevice()
{
    uint32_t                                queueFamilyPropertiesCount = 0;
    
    const std::vector<const char*>          deviceExtensions = getDeviceExtensions();
    const std::vector<const char*>          validationLayers = getValidationLayers();

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = _physicalDevice.getQueueFamilyProperties();

    vk::SurfaceKHR temporarySurface = _window->createVulkanSurface(getInstance()); // Window owns this surface instead of this function.

    // Determine the queues for graphics and present.
    uint32_t queueFamilyIndex = 0;
    vk::Bool32 presentSupport = false;
    for (const vk::QueueFamilyProperties& properties : queueFamilyProperties)
    {

        if (properties.queueFlags & vk::QueueFlagBits::eGraphics)
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
    std::vector queueCreateInfos{
        vk::DeviceQueueCreateInfo{
            {},                     // flags
            _graphicsFamilyIndex,   // queueFamilyIndex
            1,                      // queueCount
            queuePriorities,        // pQueuePriorities
        },
        vk::DeviceQueueCreateInfo{
            {},                     // flags
            _presentFamilyIndex,    // queueFamilyIndex
            1,                      // queueCount
            queuePriorities,        // pQueuePriorities
        }
    };

    queueCreateInfos.resize(areQueuesSame() ? 1 : 2);

    const vk::PhysicalDeviceFeatures features{};
    const vk::DeviceCreateInfo createInfo{
        {},                 // flags
        queueCreateInfos,   // queueCreateInfos
        validationLayers,   // pEnabledLayerNames
        deviceExtensions,   // pEnabledExtensionNames
        &features,          // pEnabledFeatures
    };

    _device = getPhysicalDevice().createDeviceUnique(createInfo);

    _graphicsQueue = getDevice().getQueue(_graphicsFamilyIndex, 0);
    _presentQueue = getDevice().getQueue(_presentFamilyIndex, 0);
}

void blRenderDevice::createCommandPool()
{
    const vk::CommandPoolCreateInfo createInfo{
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, // flags
        _graphicsFamilyIndex                                // queueFamilyIndex
    };

    _commandPool = getDevice().createCommandPoolUnique(createInfo);
}

void blRenderDevice::createAllocator()
{
    const VmaAllocatorCreateInfo createInfo{
        .flags = 0,
        .physicalDevice = (VkPhysicalDevice)getPhysicalDevice(),
        .device = (VkDevice)getDevice(),
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = nullptr,
        .instance = (VkInstance)getInstance(),
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the vulkan memory allocator!");
    }
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL blRenderDevice::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept
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