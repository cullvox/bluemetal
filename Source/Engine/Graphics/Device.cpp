
///////////////////////////////
// Headers
///////////////////////////////

#include "Core/Log.h"
#include "Device.h"

///////////////////////////////
// Disable Warnings
///////////////////////////////

// disable warnings from vk_mem_alloc warnings on platforms
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

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

GraphicsDevice::GraphicsDevice(
    GraphicsInstance*                   instance, 
    GraphicsPhysicalDevice*             physicalDevice,
    Window*                             window)
    : m_instance(instance)
    , m_physicalDevice(physicalDevice)
{
    createDevice(window);
    createCommandPool();
    createAllocator();
}

GraphicsDevice::~GraphicsDevice()
{
    vmaDestroyAllocator(m_allocator);
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    vkDestroyDevice(m_device, nullptr);
}

GraphicsPhysicalDevice* GraphicsDevice::getPhysicalDevice()
{
    return m_physicalDevice;
}

uint32_t GraphicsDevice::getGraphicsFamilyIndex()
{
    return m_graphicsFamilyIndex;
}

uint32_t GraphicsDevice::getPresentFamilyIndex()
{
    return m_presentFamilyIndex;
}

VkDevice GraphicsDevice::getHandle()
{
    return m_device;
}

VkQueue GraphicsDevice::getGraphicsQueue()
{
    return m_graphicsQueue;
}

VkQueue GraphicsDevice::getPresentQueue()
{
    return m_presentQueue;
}

VkCommandPool GraphicsDevice::getCommandPool()
{
    return m_commandPool;
}

VmaAllocator GraphicsDevice::getAllocator()
{
    return m_allocator;
}

bool GraphicsDevice::areQueuesSame()
{
    return m_graphicsFamilyIndex == m_presentFamilyIndex;
}

void GraphicsDevice::immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder)
{
    // Allocate the command buffer used to record the submission.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmd = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(m_device, &allocateInfo, &cmd) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not allocate a Vulkan command buffer!");
    }

    // Begin the command buffer.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not begin a temporary command buffer!");
    }

    // Record the commands onto the buffer.
    recorder(cmd);

    // End the command buffer.
    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not end a temporary command buffer!");
    }

    // Submit the command buffer using the graphics queue.
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not submit a temporary command buffer!");
    }

    vkQueueWaitIdle(m_graphicsQueue);
}

void GraphicsDevice::waitForDevice()
{
    vkDeviceWaitIdle(m_device);
}

std::vector<const char*> GraphicsDevice::getValidationLayers()
{
   
#ifdef BLUEMETAL_DEVELOPMENT // Disable validation layers on release.
    return {};

    
#else // Enable validation layers on debug.

    std::vector layers = // Requested layers.
    {
        "VK_LAYER_KHRONOS_validation",  
    };

    // Get the systems validation layers.
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    if (vkEnumerateDeviceLayerProperties(m_physicalDevice->getHandle(), &propertiesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan device layer properties count!");
    }

    properties.resize(propertiesCount);

    if (vkEnumerateDeviceLayerProperties(m_physicalDevice->getHandle(), &propertiesCount, properties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate Vulkan device layer properties!");
    }

    // Ensure that the requested layers are present on the system.
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

    // Found all requested layers layers!
    return layers;

#endif
}

std::vector<const char*> GraphicsDevice::getExtensions()
{

    std::vector requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    
    // Get vulkan device extensions on this system.
    uint32_t propertyCount = 0;
    std::vector<VkExtensionProperties> properties;

    if (vkEnumerateDeviceExtensionProperties(m_physicalDevice->getHandle(), nullptr, &propertyCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan device extension properties count!");
    }

    properties.resize(propertyCount);

    if (vkEnumerateDeviceExtensionProperties(m_physicalDevice->getHandle(), nullptr, &propertyCount, properties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("could not enumerate Vulkan device extension properties!");
    }
   
    // Check if our extensions are available.
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

void GraphicsDevice::createDevice(Window* window)
{   
    const std::vector<const char*> extensions = getExtensions();
    const std::vector<const char*> validationLayers = getValidationLayers();

    // Get the queue family properties of the physical device.
    uint32_t queuePropertyCount = 0;
    std::vector<VkQueueFamilyProperties> queueProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice->getHandle(), &queuePropertyCount, nullptr);

    queueProperties.resize(queuePropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice->getHandle(), &queuePropertyCount, queueProperties.data());

    // Determine what families will be dedicated to graphics and present.
    uint32_t i = 0;
    for (const VkQueueFamilyProperties& properties : queueProperties)
    {
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsFamilyIndex = i; 
        }

        VkBool32 surfaceSupported = VK_FALSE;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice->getHandle(), i, window->getSurface(), &surfaceSupported) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not check Vulkan physical device surface support!");
        }
        
        if (surfaceSupported)
        {
            m_presentFamilyIndex = i;
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
            m_graphicsFamilyIndex,                      // queueFamilyIndex
            1,                                          // queueCount
            queuePriorities,                            // pQueuePriorities
        },
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr,                                    // pNext,
            0,                                          // flags
            m_presentFamilyIndex,                       // queueFamilyIndex
            1,                                          // queueCount
            queuePriorities,                            // pQueuePriorities
        }
    };

    // Only use the unique queue indices, this is a basic way to do that.
    // In the event that we use compute, this needs to be upgraded.
    queueCreateInfos.resize(areQueuesSame() ? 1 : 2);

    // Create the device.
    const VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.pEnabledFeatures = &features;

    if (vkCreateDevice(m_physicalDevice->getHandle(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the Vulkan device!");
    }

    // Get the graphics and present queue objects.
    vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_presentFamilyIndex, 0, &m_presentQueue);

    BL_LOG(LogType::eInfo, "Created Vulkan device using {}.", m_physicalDevice->getDeviceName())
}

void GraphicsDevice::createCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = m_graphicsFamilyIndex;

    if (vkCreateCommandPool(m_device, &createInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan command pool!");
    }
}

void GraphicsDevice::createAllocator()
{
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.flags = 0;
    createInfo.physicalDevice = m_physicalDevice->getHandle();
    createInfo.device = m_device;
    createInfo.preferredLargeHeapBlockSize = 0;
    createInfo.pAllocationCallbacks = nullptr;
    createInfo.pDeviceMemoryCallbacks = nullptr;
    createInfo.pHeapSizeLimit = nullptr;
    createInfo.pVulkanFunctions = nullptr;
    createInfo.instance = m_instance->getHandle();
    createInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    createInfo.pTypeExternalMemoryHandleTypes = nullptr;

    if (vmaCreateAllocator(&createInfo, &m_allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("could not create the vulkan memory allocator!");
    }
}

} // namespace bl