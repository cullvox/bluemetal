
///////////////////////////////
// Headers
///////////////////////////////

#include "Device.h"
#include "Core/Log.h"

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
#pragma warning(disable : 4100) // unused parameter
#pragma warning(disable : 4189) // unused variables
#pragma warning(disable : 4127) // constexpr not used
#pragma warning(disable : 4324) // padded structures
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#ifdef BLUEMETAL_COMPILER_GNU
#pragma GCC diagnostic pop
#elif BLUEMETAL_COMPILER_MSVC
#pragma warning(pop)
#endif

namespace bl {

///////////////////////////////
// Classes
///////////////////////////////

GraphicsDevice::GraphicsDevice() { }

GraphicsDevice::GraphicsDevice(GraphicsDevice&& rhs)
{
    destroy();

    m_createInfo = std::move(rhs.m_createInfo);
    m_pInstance = std::move(rhs.m_pInstance);
    m_pPhysicalDevice = std::move(rhs.m_pPhysicalDevice);
    m_graphicsFamilyIndex = std::move(rhs.m_graphicsFamilyIndex);
    m_presentFamilyIndex = std::move(rhs.m_presentFamilyIndex);
    m_device = std::move(rhs.m_device);
    m_graphicsQueue = std::move(rhs.m_graphicsQueue);
    m_presentQueue = std::move(rhs.m_presentQueue);
    m_commandPool = std::move(rhs.m_commandPool);
    m_allocator = std::move(rhs.m_allocator);

    rhs.m_createInfo = {};
    rhs.m_pInstance = nullptr;
    rhs.m_pPhysicalDevice = {};
    rhs.m_graphicsFamilyIndex = 0;
    rhs.m_presentFamilyIndex = 0;
    rhs.m_device = VK_NULL_HANDLE;
    rhs.m_graphicsQueue = VK_NULL_HANDLE;
    rhs.m_presentQueue = VK_NULL_HANDLE;
    rhs.m_commandPool = VK_NULL_HANDLE;
    rhs.m_allocator = VK_NULL_HANDLE;
}

GraphicsDevice::GraphicsDevice(const GraphicsDeviceCreateInfo& createInfo) 
{ 
    if (!create(createInfo))
        throw std::runtime_error("Could not create a Graphics Device!");
}

GraphicsDevice::~GraphicsDevice() { destroy(); }

bool GraphicsDevice::create(const GraphicsDeviceCreateInfo& createInfo)
{
    m_createInfo = createInfo;
    m_pInstance = createInfo.pInstance;
    m_pPhysicalDevice = createInfo.pPhysicalDevice;

    return createDevice() && createCommandPool() && createAllocator();
}

void GraphicsDevice::destroy() noexcept
{
    if (!isCreated()) return;

    vmaDestroyAllocator(m_allocator);
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    vkDestroyDevice(m_device, nullptr);
}

bool GraphicsDevice::isCreated() const noexcept { return m_device != VK_NULL_HANDLE; }

GraphicsPhysicalDevice* GraphicsDevice::getPhysicalDevice() const
{
    assert(isCreated() && "Device must be created before the physical device can be retrieved.");

    return m_pPhysicalDevice;
}

uint32_t GraphicsDevice::getGraphicsFamilyIndex() const
{
    assert(isCreated() && "Device must be created before the graphics family index can be retrieved.");

    return m_graphicsFamilyIndex;
}

uint32_t GraphicsDevice::getPresentFamilyIndex() const
{
    assert(isCreated() && "Device must be created before the present family index can be retrieved.");

    return m_presentFamilyIndex;
}

VkDevice GraphicsDevice::getHandle() const
{
    assert(isCreated() && "Device must be created before the handle can be retrieved.");

    return m_device;
}

VkQueue GraphicsDevice::getGraphicsQueue() const
{
    assert(isCreated() && "Device must be created before the graphics queue can be retrieved.");

    return m_graphicsQueue;
}

VkQueue GraphicsDevice::getPresentQueue() const
{
    assert(isCreated() && "Device must be created before the present queue can be retrieved.");

    return m_presentQueue;
}

VkCommandPool GraphicsDevice::getCommandPool() const
{
    assert(isCreated() && "Device must be created before the command pool can be retrieved.");

    return m_commandPool;
}

VmaAllocator GraphicsDevice::getAllocator() const
{
    assert(isCreated() && "Device must be created before the allocator can be retrieved.");

    return m_allocator;
}

bool GraphicsDevice::areQueuesSame() const
{
    assert(isCreated() && "Device must be created before the queue sameness can be checked.");

    return m_graphicsFamilyIndex == m_presentFamilyIndex;
}

bool GraphicsDevice::immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder)
{

    // Allocate the command buffer used to record the submission.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmd = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(m_device, &allocateInfo, &cmd) != VK_SUCCESS) {
        blError("Could not allocate a Vulkan command buffer!");
        return false;
    }

    // Begin the command buffer.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
        blError("Could not begin a temporary command buffer!");
        return false;
    }

    // Record the commands onto the buffer.
    recorder(cmd);

    // End the command buffer.
    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        blError("Could not end a temporary command buffer!");
        return false;
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

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        blError("Could not submit a temporary command buffer!");
        return false;
    }

    return true;
}

void GraphicsDevice::waitForDevice() { vkDeviceWaitIdle(m_device); }

bool GraphicsDevice::getValidationLayers(std::vector<const char*>& outLayers)
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

    if (vkEnumerateDeviceLayerProperties(m_pPhysicalDevice->getHandle(), &propertiesCount, nullptr) != VK_SUCCESS) {
        blError("Could not get Vulkan device layer properties count!");
        return false;
    }

    properties.resize(propertiesCount);

    if (vkEnumerateDeviceLayerProperties(m_pPhysicalDevice->getHandle(), &propertiesCount, properties.data()) != VK_SUCCESS) {
        blError("Could not enumerate Vulkan device layer properties!");
        return false;
    }

    // Ensure that the requested layers are present on the system.
    for (const char* name : layers) {
        auto func = [name](const VkLayerProperties& properties) { return strcmp(properties.layerName, name) == 0; };
        
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            blError("Could not find required device layer: {}", name);
            return false;
        }
    }

    // Found all requested layers layers!
    outLayers = layers;
    return true;
#endif

}

bool GraphicsDevice::getExtensions(std::vector<const char*>& outExtensions)
{

    std::vector requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // Get vulkan device extensions on this system.
    uint32_t propertyCount = 0;
    std::vector<VkExtensionProperties> properties;

    if (vkEnumerateDeviceExtensionProperties(m_pPhysicalDevice->getHandle(), nullptr, &propertyCount, nullptr) != VK_SUCCESS) {
        blError("Could not get Vulkan device extension properties count!");
        return false;
    }

    properties.resize(propertyCount);

    if (vkEnumerateDeviceExtensionProperties(m_pPhysicalDevice->getHandle(), nullptr, &propertyCount, properties.data()) != VK_SUCCESS) {
        blError("Could not enumerate Vulkan device extension properties!");
        return false;
    }

    // Check if our extensions are available.
    for (const char* pName : requiredExtensions) {
        auto func = [pName](const VkExtensionProperties& properties){ return std::strcmp(pName, properties.extensionName) == 0; });
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            blError("could not find required instance extension: {}", pName);
            return false;
        }
    }

    outExtensions = requiredExtensions;
    return true;
}

bool GraphicsDevice::createDevice()
{
    std::vector<const char*> extensions;
    std::vector<const char*> layers;

    if (!getExtensions(extensions) || !getValidationLayers(layers))
        return false;

    // Get the queue family properties of the physical device.
    uint32_t queuePropertyCount = 0;
    std::vector<VkQueueFamilyProperties> queueProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevice->getHandle(), &queuePropertyCount, nullptr);

    queueProperties.resize(queuePropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(m_pPhysicalDevice->getHandle(), &queuePropertyCount, queueProperties.data());

    // Determine what families will be dedicated to graphics and present.
    uint32_t i = 0;
    for (const VkQueueFamilyProperties& properties : queueProperties) {
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicsFamilyIndex = i;
        }

        VkBool32 surfaceSupported = VK_FALSE;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(m_pPhysicalDevice->getHandle(), i, m_createInfo.pWindow->getSurface(), &surfaceSupported) != VK_SUCCESS) {
            blError("Could not check Vulkan physical device surface support!");
            return false;
        }

        if (surfaceSupported) {
            m_presentFamilyIndex = i;
        }

        i++;
    }

    const float queuePriorities[] = { 1.0f, 1.0f };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos {
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr, // pNext,
            0, // flags
            m_graphicsFamilyIndex, // queueFamilyIndex
            1, // queueCount
            queuePriorities, // pQueuePriorities
        },
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr, // pNext,
            0, // flags
            m_presentFamilyIndex, // queueFamilyIndex
            1, // queueCount
            queuePriorities, // pQueuePriorities
        }
    };

    // Only use the unique queue indices, this is a basic way to do that.
    // In the event that we use compute, this needs to be upgraded.
    queueCreateInfos.resize(areQueuesSame() ? 1 : 2);

    // Create the device.
    const VkPhysicalDeviceFeatures features {};
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledLayerCount = (uint32_t)layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.pEnabledFeatures = &features;

    if (vkCreateDevice(m_pPhysicalDevice->getHandle(), &createInfo, nullptr, &m_device) != VK_SUCCESS) {
        blError("Could not create the Vulkan device!");
        return false;
    }

    // Get the graphics and present queue objects.
    vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device,  m_presentFamilyIndex, 0, &m_presentQueue);

    blInfo("Created Vulkan device using {}.", m_pPhysicalDevice->getDeviceName());
    return true;
}

bool GraphicsDevice::createCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _graphicsFamilyIndex;

    if (vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        blError("Could not create a Vulkan command pool!");
        return false;
    }

    return true;
}

bool GraphicsDevice::createAllocator()
{
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.flags = 0;
    createInfo.physicalDevice = _pPhysicalDevice->getHandle();
    createInfo.device = _device;
    createInfo.preferredLargeHeapBlockSize = 0;
    createInfo.pAllocationCallbacks = nullptr;
    createInfo.pDeviceMemoryCallbacks = nullptr;
    createInfo.pHeapSizeLimit = nullptr;
    createInfo.pVulkanFunctions = nullptr;
    createInfo.instance = _pInstance->getHandle();
    createInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    createInfo.pTypeExternalMemoryHandleTypes = nullptr;

    if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS) {
        blError("Could not create the Vulkan Memory Allocator!");
        return false;
    }

    return true;
}

} // namespace bl
