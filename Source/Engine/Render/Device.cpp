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
    , _window(window)
{
    createDevice();
    createCommandPool();
    createAllocator();
}

blDevice::~blDevice()
{
    vmaDestroyAllocator(_allocator);
    vkDestroyCommandPool(_device, _commandPool, nullptr);
    vkDestroyDevice(_device, nullptr);
}

uint32_t blDevice::getGraphicsFamilyIndex() const noexcept
{
    return _graphicsFamilyIndex;
}

uint32_t blDevice::getPresentFamilyIndex() const noexcept
{
    return _presentFamilyIndex;
}

VkDevice blDevice::getDevice() const noexcept
{
    return _device;
}

VkQueue blDevice::getGraphicsQueue() const noexcept
{
    return _graphicsQueue;
}

VkQueue blDevice::getPresentQueue() const noexcept
{
    return _presentQueue;
}

VkCommandPool blDevice::getCommandPool() const noexcept
{
    return _commandPool;
}

VmaAllocator blDevice::getAllocator() const noexcept
{
    return _allocator;
}

bool blDevice::areQueuesSame() const noexcept
{
    return _graphicsFamilyIndex == _presentFamilyIndex;
}

void blDevice::immediateSubmit(const std::function<void(vk::CommandBuffer)>& recorder) const
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

void blDevice::waitForDevice() const noexcept
{
    getDevice().waitIdle();
}

std::string blDevice::getVendorName() const noexcept
{
    uint32_t vendor = _physicalDevice.getProperties().vendorID;

    switch (vendor)
    {
        case 0x1002: return "AMD";
        case 0x1010: return "ImgTec";
        case 0x10DE: return "NVIDIA";
        case 0x13B5: return "ARM";
        case 0x5143: return "Qualcomm";
        case 0x8086: return "INTEL";
        default: return "Undefined Vendor";
    }
}

std::string blDevice::getDeviceName() const noexcept
{
    return _physicalDevice.getProperties().deviceName;
}

std::vector<const char*> blDevice::getValidationLayers() const
{

}

std::vector<const char*> blDevice::getInstanceExtensions() const
{


}

void blDevice::createInstance()
{
    

    // create the windows surface
    _surface = _window->createSurface(getInstance());
    
}

void blDevice::choosePhysicalDevice()
{
    std::vector<vk::PhysicalDevice> physicalDevices = getInstance().enumeratePhysicalDevices();

    // TODO: Add a parameter: hint for selecting physical devices.
    _physicalDevice = physicalDevices[0];

    vk::PhysicalDeviceProperties properties = _physicalDevice.getProperties();

    BL_LOG(blLogType::eInfo, "using vulkan physical device: {}", properties.deviceName);
}

std::vector<const char*> blDevice::getDeviceExtensions() const
{
    std::vector requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    
    std::vector<vk::ExtensionProperties> extensionProperties = _physicalDevice.enumerateDeviceExtensionProperties();

    for (const char* pName : requiredExtensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
                [pName](const vk::ExtensionProperties& properties)
                {
                    return std::strcmp(pName, properties.extensionName) == 0; 
                }) == extensionProperties.end())
        {
            BL_LOG(blLogType::eFatal, "could not find required instance extension: {}", pName);
        }
    }

    return requiredExtensions;
}

void blDevice::createDevice()
{   
    const std::vector<const char*> deviceExtensions = getDeviceExtensions();
    const std::vector<const char*> validationLayers =
#ifdef BLUEMETAL_DEBUG
    getValidationLayers();
#else
    {};
#endif

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = _physicalDevice.getQueueFamilyProperties();

    // Determine the queues for graphics and present.
    uint32_t queueFamilyIndex = 0;
    for (const vk::QueueFamilyProperties& properties : queueFamilyProperties)
    {
        if (properties.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            _graphicsFamilyIndex = queueFamilyIndex; 
        }

        if (_physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, getSurface()))
        {
            _presentFamilyIndex = queueFamilyIndex;
        }

        queueFamilyIndex++;
    }

    const float queuePriorities[] = { 1.0f, 1.0f };
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos
    {
        {
            {},                     // flags
            _graphicsFamilyIndex,   // queueFamilyIndex
            1,                      // queueCount
            queuePriorities,        // pQueuePriorities
        },
        {
            {},                     // flags
            _presentFamilyIndex,    // queueFamilyIndex
            1,                      // queueCount
            queuePriorities,        // pQueuePriorities
        }
    };

    // Only use the unique queue indices, this is a basic way to do that
    // In the event that we use compute, this needs to be upgraded
    queueCreateInfos.resize(areQueuesSame() ? 1 : 2);

    const vk::PhysicalDeviceFeatures features{};
    const vk::DeviceCreateInfo createInfo
    {
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

void blDevice::createCommandPool()
{
    const vk::CommandPoolCreateInfo createInfo
    {
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, // flags
        _graphicsFamilyIndex                                // queueFamilyIndex
    };

    _commandPool = getDevice().createCommandPoolUnique(createInfo);
}

void blDevice::createAllocator()
{
    const VmaAllocatorCreateInfo createInfo{
        0,                      // flags
        getPhysicalDevice(),    // physicalDevice
        getDevice(),            // device
        0,                      // preferredLargeHeapBlockSize
        nullptr,                // pAllocationCallbacks
        nullptr,                // pDeviceMemoryCallbacks
        nullptr,                // pHeapSizeLimit
        nullptr,                // pVulkanFunctions
        getInstance(),          // instance
        VK_API_VERSION_1_2,     // vulkanApiVersion
        nullptr,                // pTypeExternalMemoryHandleTypes
    };

    VmaAllocator allocator = {};
    if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("could not create the vulkan memory allocator!");
    }

    _allocator = std::shared_ptr<VmaAllocator>(allocator, [](VmaAllocator allocator){ vmaDestroyAllocator(allocator); });
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL blDevice::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept
{
    (void)pUserData;
    (void)type;

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