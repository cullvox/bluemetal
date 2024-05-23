#include "Device.h"
#include "Core/Print.h"

// VMA has a lot of warnings on a lot of different platforms.
// Disable warnings from vk_me_alloc.h warnings on platforms.

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

Device::Device(Instance* instance, PhysicalDevice* physicalDevice)
    : _instance(instance)
    , _physicalDevice(physicalDevice)
{
    CreateDevice();
    CreateCommandPool();
    CreateAllocator();

    _descriptorSetLayoutCache = std::make_unique<DescriptorSetLayoutCache>(this);
    _pipelineLayoutCache = std::make_unique<PipelineLayoutCache>(this);
    _descriptorSetAllocator = std::make_unique<DescriptorSetAllocator>(this, 1024, DescriptorRatio::Default());
}

Device::~Device() { 
    vmaDestroyAllocator(_allocator);
    vkDestroyCommandPool(_device, _commandPool, nullptr);
    vkDestroyDevice(_device, nullptr);
}

Instance* Device::GetInstance() const
{
    return _instance;
}

PhysicalDevice* Device::GetPhysicalDevice() const
{
    return _physicalDevice;
}

uint32_t Device::GetGraphicsFamilyIndex() const
{
    return _graphicsFamilyIndex;
}

uint32_t Device::GetPresentFamilyIndex() const
{
    return _presentFamilyIndex;
}

bool Device::GetAreQueuesSame() const
{
    return _graphicsFamilyIndex == _presentFamilyIndex;
}

VkQueue Device::GetGraphicsQueue() const
{
    return _graphicsQueue;
}

VkQueue Device::GetPresentQueue() const
{
    return _presentQueue;
}

VkDevice Device::Get() const
{
    return _device;
}

VkCommandPool Device::GetCommandPool() const
{
    return _commandPool;
}

VmaAllocator Device::GetAllocator() const
{
    return _allocator;
}

void Device::ImmediateSubmit(const std::function<void(VkCommandBuffer)>& recorder)
{
    // Allocate the command buffer used to record the submission.
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = _commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmd = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateCommandBuffers(_device, &allocateInfo, &cmd))

    // Begin the command buffer.
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    // Record the commands onto the buffer.
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));
    recorder(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd))

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

    VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE))

    WaitForDevice();
}

void Device::WaitForDevice() const 
{ 
    vkDeviceWaitIdle(_device); 
}

VkDescriptorSetLayout Device::CacheDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    return _descriptorSetLayoutCache->Acquire(bindings);
}

VkPipelineLayout Device::CachePipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptors, const std::vector<VkPushConstantRange>& pushConstants)
{
    return _pipelineLayoutCache->Acquire(descriptors, pushConstants);
}

VkDescriptorSet Device::AllocateDescriptorSet(VkDescriptorSetLayout layout)
{
    return _descriptorSetAllocator->Allocate(layout);
}

std::vector<VkDescriptorSet> Device::AllocateDescriptorSets(VkDescriptorSetLayout layout, uint32_t count)
{
    std::vector<VkDescriptorSet> sets;
    sets.reserve(count);

    for (uint32_t i = 0; i < count; i++) 
        sets.push_back(_descriptorSetAllocator->Allocate(layout));

    return sets;
}

std::vector<const char*> Device::GetValidationLayers()
{
    const auto& layers = GraphicsConfig::validationLayers;

    // Get the systems validation layers.
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    VK_CHECK(vkEnumerateDeviceLayerProperties(_physicalDevice->Get(), &propertiesCount, nullptr))
    properties.resize(propertiesCount);
    VK_CHECK(vkEnumerateDeviceLayerProperties(_physicalDevice->Get(), &propertiesCount, properties.data()))

    // Ensure that the requested layers are present on the system.
    for (const char* name : layers) {
        if (!std::any_of(properties.begin(), properties.end(), 
                [name](const auto& properties){ 
                    return strcmp(name, properties.layerName) == 0; 
                })) {
            throw std::runtime_error("Could not find a required device layer!");
        }
    }

    return layers;
}

std::vector<const char*> Device::GetExtensions()
{
    // The engines required device extensions.
    std::vector requiredExtensions = { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    // Get vulkan device extensions on this system.
    uint32_t propertyCount = 0;
    std::vector<VkExtensionProperties> properties;

    VK_CHECK(vkEnumerateDeviceExtensionProperties(_physicalDevice->Get(), nullptr, &propertyCount, nullptr))
    properties.resize(propertyCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(_physicalDevice->Get(), nullptr, &propertyCount, properties.data()))

    // Ensure the required extensions are available.
    for (const char* pName : requiredExtensions) {
        if (!std::any_of(properties.begin(), properties.end(), 
                [pName](const auto& properties){ 
                    return std::strcmp(pName, properties.extensionName) == 0; 
                })) {
            throw std::runtime_error("Could not find required instance extension: {}");
        }
    }

    return requiredExtensions;
}

void Device::CreateDevice()
{
    std::vector<const char*> extensions = GetExtensions();
    std::vector<const char*> layers{};

    if (_instance->GetValidationEnabled())
        layers = GetValidationLayers();

    // Get the queue family properties of the physical device.
    uint32_t queuePropertyCount = 0;
    std::vector<VkQueueFamilyProperties> queueProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice->Get(), &queuePropertyCount, nullptr);
    queueProperties.resize(queuePropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice->Get(), &queuePropertyCount, queueProperties.data());

    // Determine what families will be dedicated to graphics and present.
    uint32_t i = 0;

    // Use a temporary window and surface for surface support checking.
    Window::UseTemporarySurface(_instance, [&](VkSurfaceKHR surface)
    {

        // Check for queue usage.
        for (const VkQueueFamilyProperties& properties : queueProperties) 
        {
            if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                _graphicsFamilyIndex = i;
            }

            VkBool32 surfaceSupported = VK_FALSE;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice->Get(), i, surface, &surfaceSupported))
            
            if (surfaceSupported) 
            {
                _presentFamilyIndex = i;
            }

            i++;
        }
    });

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
    queueCreateInfos.resize(GetAreQueuesSame() ? 1 : 2);

    const VkPhysicalDeviceFeatures features = {};

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

    VK_CHECK(vkCreateDevice(_physicalDevice->Get(), &createInfo, nullptr, &_device))

    volkLoadDevice(_device);  // Load the next set of vulkan functions based on the device.

    // Get the graphics and present queue objects.
    vkGetDeviceQueue(_device, _graphicsFamilyIndex, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device,  _presentFamilyIndex, 0, &_presentQueue);

    blInfo("Created the Vulkan device using: {}", _physicalDevice->GetDeviceName());
}

void Device::CreateCommandPool()
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = _graphicsFamilyIndex;

    VK_CHECK(vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPool))
}

void Device::CreateAllocator()
{
    VmaVulkanFunctions functions = {};
    functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo createInfo = {};
    createInfo.flags = 0;
    createInfo.physicalDevice = _physicalDevice->Get();
    createInfo.device = _device;
    createInfo.preferredLargeHeapBlockSize = 0;
    createInfo.pAllocationCallbacks = nullptr;
    createInfo.pDeviceMemoryCallbacks = nullptr;
    createInfo.pHeapSizeLimit = nullptr;
    createInfo.pVulkanFunctions = &functions;
    createInfo.instance = _instance->Get();
    createInfo.vulkanApiVersion = GraphicsConfig::apiVersion;
    createInfo.pTypeExternalMemoryHandleTypes = nullptr;

    VK_CHECK(vmaCreateAllocator(&createInfo, &_allocator))
}

} // namespace bl
