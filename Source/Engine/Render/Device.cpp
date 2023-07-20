#include "Device.h"
#include "Core/Log.h"

// disable vk_mem_alloc warnings on platforms
#ifdef BLUEMETAL_COMPILER_GNU
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-variable"
  #pragma GCC diagnostic ignored "-Wparentheses"
#elif BLUEMETAL_COMPILER_MSVC
  #pragma warning(push)
  #pragma warning(disable: 4100) // unused parameter
  #pragma warning(disable: 4189) // unused variables
  #pragma warning(disable: 4127) // constexpr not used
  #pragma warning(disable: 4324) // padded structures
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#ifdef BLUEMETAL_COMPILER_GNU
  #pragma GCC diagnostic pop
#elif BLUEMETAL_COMPILER_MSVC
  #pragma warning(pop)
#endif

blDevice::blDevice(std::shared_ptr<blInstance> instance, std::shared_ptr<blWindow> window)
    : _instance(instance)
{
    createDevice(window);
    createCommandPool();
    createAllocator();
}

blDevice::~blDevice()
{
    vmaDestroyAllocator(_allocator);
    vkDestroyCommandPool(_device, _commandPool, nullptr);
    vkDestroyDevice(_device, nullptr);
}

uint32_t blDevice::getGraphicsFamilyIndex()
{
    return _graphicsFamilyIndex;
}

uint32_t blDevice::getPresentFamilyIndex()
{
    return _presentFamilyIndex;
}

VkInstance blDevice::getInstance()
{
    return _instance->getInstance();
}

VkPhysicalDevice blDevice::getPhysicalDevice()
{
    return _instance->getPhysicalDevice();
}

VkDevice blDevice::getDevice()
{
    return _device;
}

VkQueue blDevice::getGraphicsQueue()
{
    return _graphicsQueue;
}

VkQueue blDevice::getPresentQueue()
{
    return _presentQueue;
}

VkCommandPool blDevice::getCommandPool()
{
    return _commandPool;
}

VmaAllocator blDevice::getAllocator()
{
    return _allocator;
}

bool blDevice::areQueuesSame()
{
    return _graphicsFamilyIndex == _presentFamilyIndex;
}

void blDevice::immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder)
{
    const VkCommandBufferAllocateInfo allocateInfo
    {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // sType
        nullptr,                                        // pNext
        _commandPool,                                   // commandPool 
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,                // level
        1                                               // commandBufferCount
    };

    VkCommandBuffer cmd{};
    vkAllocateCommandBuffers(_device, &allocateInfo, &cmd);

    const VkCommandBufferBeginInfo beginInfo
    {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,    // sType
        nullptr,                                        // pNext
        0,                                              // flags
        nullptr                                         // pInheritanceInfo
    };

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not begin a temporary command buffer!");
    }

    recorder(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not end a temporary command buffer!");
    }

    const VkSubmitInfo submitInfo
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType
        nullptr,                        // pNext
        0,                              // waitSemaphoreCount
        nullptr,                        // pWaitSemaphores
        nullptr,                        // pWaitDstStageMask
        1,                              // commandBufferCount
        &cmd,                           // pCommandBuffers
        0,                              // signalSemaphoreCount
        nullptr                         // pSignalSemaphores
    };

    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not submit a temporary command buffer!");
    }

    vkQueueWaitIdle(_graphicsQueue);
}

void blDevice::waitForDevice()
{
    vkDeviceWaitIdle(_device);
}

std::vector<const char*> blDevice::getValidationLayers()
{
    // disable validation layers on release
    if (BLUEMETAL_DEVELOPMENT) return {};

    std::vector<const char*> layers = 
    {
        "VK_LAYER_KHRONOS_validation",  
    };

    // get the systems validation layer info
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    if (vkEnumerateDeviceLayerProperties(_instance->getPhysicalDevice(), &propertiesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan device layer properties count!");
    }

    properties.resize(propertiesCount);

    if (vkEnumerateDeviceLayerProperties(_instance->getPhysicalDevice(), &propertiesCount, properties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate Vulkan device layer properties!");
    }

    // ensure that the requested layers are present on the system
    for (const char* name : layers)
    {
        if (std::find_if(properties.begin(), properties.end(), 
                [name](const VkLayerProperties& properties)
                { 
                    return strcmp(properties.layerName, name) == 0; 
                }
            ) == properties.end())
        {
            throw std::runtime_error(fmt::format("Could not find required device layer: {}", name));
        }
    }

    // found all layers!
    return layers;
}

std::vector<const char*> blDevice::getExtensions()
{
    std::vector requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    
    // get vulkan device extensions
    uint32_t propertyCount = 0;
    std::vector<VkExtensionProperties> properties;

    if (vkEnumerateDeviceExtensionProperties(_instance->getPhysicalDevice(), nullptr, &propertyCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan device extension properties count!");
    }

    properties.resize(propertyCount);

    if (vkEnumerateDeviceExtensionProperties(_instance->getPhysicalDevice(), nullptr, &propertyCount, properties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("could not enumerate Vulkan device extension properties!");
    }
   
    // check if our extensions are on the system
    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(properties.begin(), properties.end(), 
                [pName](const VkExtensionProperties& properties)
                {
                    return std::strcmp(pName, properties.extensionName) == 0; 
                }) == properties.end())
        {
            throw std::runtime_error(fmt::format("could not find required instance extension: {}", pName));
        }
    }

    return requiredExtensions;
}

void blDevice::createDevice(std::shared_ptr<blWindow> window)
{   
    const std::vector<const char*> extensions = getExtensions();
    const std::vector<const char*> validationLayers = getValidationLayers();

    // get the queue family properties of the physical device
    uint32_t queuePropertyCount = 0;
    std::vector<VkQueueFamilyProperties> queueProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(_instance->getPhysicalDevice(), &queuePropertyCount, nullptr);

    queueProperties.resize(queuePropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(_instance->getPhysicalDevice(), &queuePropertyCount, queueProperties.data());

    // determine what families will be dedicated to graphics and present
    uint32_t i = 0;
    for (const VkQueueFamilyProperties& properties : queueProperties)
    {
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            _graphicsFamilyIndex = i; 
        }

        VkBool32 surfaceSupported = VK_FALSE;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(_instance->getPhysicalDevice(), i, window->getSurface(), &surfaceSupported) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not check Vulkan physical device surface support!");
        }
        
        if (surfaceSupported)
        {
            _presentFamilyIndex = i;
        }

        i++;
    }

    const float queuePriorities[] = { 1.0f, 1.0f };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos
    {
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr,                                    // pNext,
            0,                                          // flags
            _graphicsFamilyIndex,                       // queueFamilyIndex
            1,                                          // queueCount
            queuePriorities,                            // pQueuePriorities
        },
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr,                                    // pNext,
            0,                                          // flags
            _presentFamilyIndex,                        // queueFamilyIndex
            1,                                          // queueCount
            queuePriorities,                            // pQueuePriorities
        }
    };

    // only use the unique queue indices, this is a basic way to do that
    // in the event that we use compute, this needs to be upgraded
    queueCreateInfos.resize(areQueuesSame() ? 1 : 2);

    const VkPhysicalDeviceFeatures features{};
    const VkDeviceCreateInfo createInfo
    {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,   // sType
        nullptr,                                // pNext
        0,                                      // flags
        (uint32_t)queueCreateInfos.size(),      // queueCreateInfoCount
        queueCreateInfos.data(),                // pQueueCreateInfos
        (uint32_t)validationLayers.size(),      // enabledLayerCount
        validationLayers.data(),                // ppEnabledLayerNames
        (uint32_t)extensions.size(),            // enabledExtensionCount
        extensions.data(),                      // ppEnabledExtensionNames
        &features,                              // pEnabledFeatures
    };

    if (vkCreateDevice(_instance->getPhysicalDevice(), &createInfo, nullptr, &_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the Vulkan device!");
    }

    vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, _presentFamilyIndex, 0, &_presentQueue);
}

void blDevice::createCommandPool()
{
    const VkCommandPoolCreateInfo createInfo
    {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,         // sType
        nullptr,                                            // pNext
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,    // flags
        _graphicsFamilyIndex                                // queueFamilyIndex
    };

    if (vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan command pool!");
    }
}

void blDevice::createAllocator()
{
    const VmaAllocatorCreateInfo createInfo
    {
        0,                              // flags
        _instance->getPhysicalDevice(), // physicalDevice
        _device,                        // device
        0,                              // preferredLargeHeapBlockSize
        nullptr,                        // pAllocationCallbacks
        nullptr,                        // pDeviceMemoryCallbacks
        nullptr,                        // pHeapSizeLimit
        nullptr,                        // pVulkanFunctions
        _instance->getInstance(),       // instance
        VK_API_VERSION_1_2,             // vulkanApiVersion
        nullptr,                        // pTypeExternalMemoryHandleTypes
    };

    if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("could not create the vulkan memory allocator!");
    }
}