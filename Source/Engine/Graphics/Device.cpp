#include "Device.h"
#include "Core/Print.h"

// Disable warnings from vk_me_alloc.h warnings on platforms.
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

GfxDevice::GfxDevice(const CreateInfo& createInfo)
    : _instance(createInfo.instance)
    , _physicalDevice(createInfo.physicalDevice)
{
    createDevice(createInfo.window);
    createCommandPool();
    createAllocator();
}

GfxDevice::~GfxDevice() { 
    vmaDestroyAllocator(_allocator);
    vkDestroyCommandPool(_device, _commandPool, nullptr);
    vkDestroyDevice(_device, nullptr);

    _device = VK_NULL_HANDLE;
    _commandPool = VK_NULL_HANDLE;
    _allocator = VK_NULL_HANDLE;
}

std::shared_ptr<GfxPhysicalDevice> GfxDevice::getPhysicalDevice() const
{
    return _physicalDevice;
}

uint32_t GfxDevice::getGraphicsFamilyIndex() const
{
    return _graphicsFamilyIndex;
}

uint32_t GfxDevice::getPresentFamilyIndex() const
{
    return _presentFamilyIndex;
}

VkDevice GfxDevice::get() const
{
    return _device;
}

VkQueue GfxDevice::getGraphicsQueue() const
{
    return _graphicsQueue;
}

VkQueue GfxDevice::getPresentQueue() const
{
    return _presentQueue;
}

VkCommandPool GfxDevice::getCommandPool() const
{
    return _commandPool;
}

VmaAllocator GfxDevice::getAllocator() const
{
    return _allocator;
}

bool GfxDevice::areQueuesSame() const
{
    return _graphicsFamilyIndex == _presentFamilyIndex;
}

bool GfxDevice::immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder)
{

    // Allocate the command buffer used to record the submission.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmd = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(_device, &allocateInfo, &cmd) != VK_SUCCESS) {
        blError("Could not allocate a temporary VkCommandBuffer!");
        return false;
    }

    // Begin the command buffer.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
        blError("Could not begin a temporary VkCommandBuffer!");
        return false;
    }

    // Record the commands onto the buffer.
    recorder(cmd);

    // End the command buffer.
    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        blError("Could not end a temporary VkCommandBuffer!");
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

    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        blError("Could not submit a temporary VkCommandBuffer!");
        return false;
    }

    return true;
}

void GfxDevice::waitForDevice() const 
{ 
    vkDeviceWaitIdle(_device); 
}

std::vector<const char*> GfxDevice::getValidationLayers()
{
    // The engines required layers.
    std::vector layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    // Get the systems validation layers.
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    if (vkEnumerateDeviceLayerProperties(_physicalDevice->get(), &propertiesCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get VkDevice layer properties count!");
    }

    properties.resize(propertiesCount);

    if (vkEnumerateDeviceLayerProperties(_physicalDevice->get(), &propertiesCount, properties.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not enumerate VkDevice layer properties!");
    }

    // Ensure that the requested layers are present on the system.
    for (const char* name : layers) {
        auto func = [name](const VkLayerProperties& properties) { return strcmp(properties.layerName, name) == 0; };
        
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            throw std::runtime_error("Could not find a required device layer!");
        }
    }

    // Found all requested layers layers!
    return std::move(layers);
}

std::vector<const char*> GfxDevice::getExtensions()
{
    // The engines required device extensions.
    std::vector requiredExtensions = { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    // Get vulkan device extensions on this system.
    uint32_t propertyCount = 0;
    std::vector<VkExtensionProperties> properties;

    if (vkEnumerateDeviceExtensionProperties(_physicalDevice->get(), nullptr, &propertyCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get VkDevice extension properties count!");
    }

    properties.resize(propertyCount);

    if (vkEnumerateDeviceExtensionProperties(_physicalDevice->get(), nullptr, &propertyCount, properties.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not enumerate VkDevice extension properties!");
    }

    // Ensure the required extensions are available.
    for (const char* pName : requiredExtensions) {
        auto func = [pName](const VkExtensionProperties& properties){ return std::strcmp(pName, properties.extensionName) == 0; };
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            throw std::runtime_error("Could not find required instance extension: {}");
        }
    }

    return std::move(requiredExtensions);
}

void GfxDevice::createDevice(std::shared_ptr<Window> window)
{
    std::vector<const char*> extensions = std::move(getExtensions());
    std::vector<const char*> layers = std::move(getValidationLayers());

    // Get the queue family properties of the physical device.
    uint32_t queuePropertyCount = 0;
    std::vector<VkQueueFamilyProperties> queueProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice->get(), &queuePropertyCount, nullptr);

    queueProperties.resize(queuePropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice->get(), &queuePropertyCount, queueProperties.data());

    // Determine what families will be dedicated to graphics and present.
    uint32_t i = 0;
    for (const VkQueueFamilyProperties& properties : queueProperties) {
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            _graphicsFamilyIndex = i;
        }

        VkBool32 surfaceSupported = VK_FALSE;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice->get(), i, window->getSurface(), &surfaceSupported) != VK_SUCCESS) {
            throw std::runtime_error("Could not check VkPhysicalDevice surface support!");
        }

        if (surfaceSupported) {
            _presentFamilyIndex = i;
        }

        i++;
    }

    const float queuePriorities[] = { 1.0f, 1.0f };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos {
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr, // pNext,
            0, // flags
            _graphicsFamilyIndex, // queueFamilyIndex
            1, // queueCount
            queuePriorities, // pQueuePriorities
        },
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr, // pNext,
            0, // flags
            _presentFamilyIndex, // queueFamilyIndex
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

    if (vkCreateDevice(_physicalDevice->get(), &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Vulkan device!");
    }

    // Get the graphics and present queue objects.
    vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device,  _presentFamilyIndex, 0, &_presentQueue);

    blInfo("Created the Vulkan device using: {}", _physicalDevice->getDeviceName());
}

void GfxDevice::createCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _graphicsFamilyIndex;

    if (vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Vulkan command pool!");
    }
}

void GfxDevice::createAllocator()
{
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.flags = 0;
    createInfo.physicalDevice = _physicalDevice->get();
    createInfo.device = _device;
    createInfo.preferredLargeHeapBlockSize = 0;
    createInfo.pAllocationCallbacks = nullptr;
    createInfo.pDeviceMemoryCallbacks = nullptr;
    createInfo.pHeapSizeLimit = nullptr;
    createInfo.pVulkanFunctions = nullptr;
    createInfo.instance = _instance->get();
    createInfo.vulkanApiVersion = GfxInstance::getApiVersion();
    createInfo.pTypeExternalMemoryHandleTypes = nullptr;

    if (vmaCreateAllocator(&createInfo, &_allocator) != VK_SUCCESS) {
        throw std::runtime_error("Could not create the Vulkan Memory Allocator!");
    }
}

} // namespace bl
