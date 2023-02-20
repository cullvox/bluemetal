#include "spdlog/common.h"
#include "vulkan/vulkan_core.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <stdexcept>

#include <spdlog/spdlog.h>

#define VMA_IMPLEMENTATION

#include "core/Version.hpp"
#include "RenderDevice.hpp"


CRenderDevice::CRenderDevice(IWindow* pWindow, CConfig* pConfig)
{
    CreateInstance();
    ChoosePhysicalDevice();
    CreateDevice();
    CreateAllocator();
}

CRenderDevice::~CRenderDevice()
{
    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

VkInstance CRenderDevice::GetInstance() const noexcept
{
    return m_instance;
}

VkPhysicalDevice CRenderDevice::GetPhysicalDevice() const noexcept
{
    return m_physicalDevice;
}

uint32_t CRenderDevice::GetGraphicsFamilyIndex() const noexcept
{
    return m_graphicsFamilyIndex;
}

uint32_t CRenderDevice::GetPresentFamilyIndex() const noexcept
{
    return m_presentFamilyIndex;
}

VkDevice CRenderDevice::GetDevice() const noexcept
{
    return m_device;
}

VkQueue CRenderDevice::GetGraphicsQueue() const noexcept
{
    return m_graphicsQueue;
}

VkQueue CRenderDevice::GetPresentQueue() const noexcept
{
    return m_presentQueue;
}

VmaAllocator CRenderDevice::GetAllocator() const noexcept
{
    return m_allocator;
}

std::vector<const char*> CRenderDevice::GetValidationLayers() const
{
    std::vector<const char*> requiredLayers = {
        "VK_LAYER_KHRONOS_validation",  
    };

    uint32_t layerPropertiesCount = 0;
    if (vkEnumerateInstanceLayerProperties(&layerPropertiesCount, nullptr) != VK_SUCCESS)
    {
        spdlog::log(spdlog::level::err, "Could not enumerate validation layers!");
        return {};
    }

    std::vector<VkLayerProperties> layerProperties;
    if (vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties.data()) != VK_SUCCESS)
    {
        spdlog::log(spdlog::level::err, "Could not enumerate validation layers!");
        return {};
    }

    for (const char* name : requiredLayers)
    {
        if (std::find_if(layerProperties.begin(), layerProperties.end(), 
                [name](const VkLayerProperties& properties){ 
                    return std::strcmp(properties.layerName, name) != 0; }) != layerProperties.end())
        {
            spdlog::log(spdlog::level::err, "Could not find required validation layer: {}", name);
            return {};
        }
    }

    return requiredLayers;
}

std::vector<const char*> CRenderDevice::GetInstanceExtensions() const
{
    std::vector<const char*> requiredExtensions = {
        // Platform::GetInstanceSurfaceExtension(),
#ifndef NDEBUG
        "VK_EXT_debug_utils"
#endif
    };

    uint32_t extensionPropertiesCount = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, nullptr) != VK_SUCCESS)
    {
        spdlog::log(spdlog::level::err, "Could not enumerate instance extension properties!");
        return {};
    }

    std::vector<VkExtensionProperties> extensionProperties{};
    extensionProperties.resize(extensionPropertiesCount);

    if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, extensionProperties.data()) != VK_SUCCESS)
    {
        spdlog::log(spdlog::level::err, "Could not enumerate instance extension properties!");
        return {};
    }

    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
        [pName](const VkExtensionProperties& properties){
                return std::strcmp(pName, properties.extensionName) == 0; }) != extensionProperties.end())
        {
            spdlog::log(spdlog::level::err, "Could not find required instance extension: {}", pName);
            return {};
        }
    }

    return requiredExtensions;
}

void CRenderDevice::CreateInstance()
{
    spdlog::log(spdlog::level::info, "Creating the vulkan instance.");

    const VkApplicationInfo applicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = applicationName.data(),
        .applicationVersion = VK_MAKE_VERSION(applicationVersionMajor, applicationVersionMinor, applicationVersionPatch),
        .pEngineName = engineName.data(),
        .engineVersion = VK_MAKE_VERSION(engineVersionMajor, engineVersionMinor, engineVersionPatch),
        .apiVersion = VK_API_VERSION_1_2,
    };

    std::vector<const char*> instanceExtensions = GetInstanceExtensions();
    std::vector<const char*> validationLayers = GetValidationLayers();

    const VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = (uint32_t)validationLayers.size(),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = (uint32_t)instanceExtensions.size(),
        .ppEnabledExtensionNames = instanceExtensions.data(),
    };

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan instance!");
    }
}

void CRenderDevice::ChoosePhysicalDevice()
{
    /* Find the physical devices. */
    uint32_t physicalDevicesCount;
    if (vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate physical devices!");
    }

    std::vector<VkPhysicalDevice> physicalDevices{};
    physicalDevices.resize(physicalDevicesCount);
    
    if (vkEnumeratePhysicalDevices(m_instance, &physicalDevicesCount, physicalDevices.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate physical devices!");
    }

    /* Determine if we are going to use */
    const int physicalDeviceIndex = m_pConfig->GetIntOrSetDefault("renderer.vulkan.physicalDevice", 0);
    m_physicalDevice = physicalDevices[physicalDeviceIndex];

    VkPhysicalDeviceProperties physicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

    spdlog::log(spdlog::level::info, "Using vulkan physical device: {}", physicalDeviceProperties.deviceName);
}

std::vector<const char*> CRenderDevice::GetDeviceExtensions() const
{

    std::vector<const char*> requiredExtensions{
    };

    /* Find the device extensions. */
    uint32_t deviceExtensionsCount = 0;
    if (vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionsCount, nullptr) != VK_SUCCESS)
    {
        spdlog::log(spdlog::level::err, "Could not enumerate vulkan device extensions!");
        return {};
    }

    std::vector<VkExtensionProperties> extensionProperties{};
    extensionProperties.resize(deviceExtensionsCount);
    
    if (vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionsCount, extensionProperties.data()) != VK_SUCCESS)
    {
        spdlog::log(spdlog::level::err, "Could not enumerate vulkan device extensions!");
        return {};
    }

    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
        [pName](const VkExtensionProperties& properties){
                return std::strcmp(pName, properties.extensionName) == 0; }) != extensionProperties.end())
        {
            spdlog::log(spdlog::level::err, "Could not find required instance extension: {}", pName);
            return {};
        }
    }

    return requiredExtensions;
}

void CRenderDevice::CreateDevice()
{
    /* Enumerate the physical device queues. */
    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertiesCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties{};
    queueFamilyProperties.resize(queueFamilyPropertiesCount);

    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

    /* Create a temporary surface for device creation. 
        A CSwapchain will be created later to complete the actual surface. */
    VkSurfaceKHR temporarySurface = m_pWindow->CreateSurface(m_instance);

    /* Determine a queue for a graphics rendering. */
    uint32_t queueFamilyIndex = 0;
    VkBool32 presentSupport = false;
    for (const VkQueueFamilyProperties& properties : queueFamilyProperties)
    {
        /* Check if queue supports graphics. */
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsFamilyIndex = queueFamilyIndex; 
        }

        /* See if present was properly supported. */
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
        (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_graphicsFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        },
        (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_presentFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        }
    };

    std::vector<const char*> validationLayers = GetValidationLayers();
    std::vector<const char*> deviceExtensions = GetDeviceExtensions();

    VkPhysicalDeviceFeatures features{};

    const VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = (uint32_t)validationLayers.size(),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = (uint32_t)deviceExtensions.size(),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &features,
    };

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan logical device!");
    }

    vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_presentFamilyIndex, 0, &m_presentQueue);
}

void CRenderDevice::CreateAllocator()
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
        throw std::runtime_error("Could not create the vulkan memory allocator!");
    }
}