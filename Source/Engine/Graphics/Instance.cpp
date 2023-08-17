#include "Instance.h"
#include "Core/Version.h"
#include "Core/Log.h"

#define VK_CHECK(result) if (result != VK_SUCCESS) return false;

namespace bl
{

GraphicsInstance::GraphicsInstance()
{
}

GraphicsInstance::GraphicsInstance(GraphicsInstance&& rhs)
{
    destroy();

    m_createInfo = std::move(rhs.m_createInfo);
    m_instance = std::move(rhs.m_instance);
    m_createDebugUtilsMessengerEXT = std::move(rhs.m_createDebugUtilsMessengerEXT);
    m_destroyDebugUtilsMessengerEXT = std::move(rhs.m_destroyDebugUtilsMessengerEXT);
    m_messenger = std::move(rhs.m_messenger);
    m_physicalDevices = std::move(rhs.m_physicalDevices);
}

GraphicsInstance& GraphicsInstance::operator=(GraphicsInstance&& rhs)
{
    destroy();

    m_createInfo = std::move(rhs.m_createInfo);
    m_instance = std::move(rhs.m_instance);
    m_createDebugUtilsMessengerEXT = std::move(rhs.m_createDebugUtilsMessengerEXT);
    m_destroyDebugUtilsMessengerEXT = std::move(rhs.m_destroyDebugUtilsMessengerEXT);
    m_messenger = std::move(rhs.m_messenger);
    m_physicalDevices = std::move(rhs.m_physicalDevices);

    /// Clear the values so a call to rhs.isCreated returns false.
    rhs.m_createInfo = {};
    rhs.m_instance = VK_NULL_HANDLE;
    rhs.m_createDebugUtilsMessengerEXT = nullptr;
    rhs.m_destroyDebugUtilsMessengerEXT = nullptr;
    rhs.m_messenger = VK_NULL_HANDLE;
    rhs.m_physicalDevices.clear();

    return *this;
}

GraphicsInstance::GraphicsInstance(const GraphicsInstanceCreateInfo& createInfo)
{
    if (!create(createInfo))
        throw std::runtime_error(m_err.c_str());
}

GraphicsInstance::~GraphicsInstance()
{
    destroy();
}

bool GraphicsInstance::create(const GraphicsInstanceCreateInfo& createInfo)
{
    m_createInfo = createInfo;

    createInstance();
    createPhysicalDevices();

    return true;
}

void GraphicsInstance::destroy() noexcept
{
    if (!isCreated()) return;

#ifdef BLUEMETAL_DEVELOPMENT
    m_destroyDebugUtilsMessengerEXT(m_instance, m_messenger, nullptr);
#endif

    vkDestroyInstance(m_instance, nullptr);
}

bool GraphicsInstance::isCreated() const noexcept
{
    return m_instance != VK_NULL_HANDLE;
}

VkInstance GraphicsInstance::getHandle() const
{
    assert(isCreated() && "Instance must be created before a handle can be retrieved.");

    return m_instance;
}

std::vector<GraphicsPhysicalDevice*> GraphicsInstance::getPhysicalDevices() const
{
    assert(isCreated() && "Instance must be created before physical devices can be retrieved.");

    // Get the raw pointer values from the unique pointer.
    std::vector<GraphicsPhysicalDevice*> physicalDevices;
    std::transform(m_physicalDevices.begin(), m_physicalDevices.end(), std::back_inserter(physicalDevices),
        [](auto& pd){ return pd.get(); });

    return physicalDevices;
}

bool GraphicsInstance::getExtensionsForSDL(std::vector<const char*>& outExtensions)
{

    // Create a temporary window to get extensions from SDL.
    SDL_Window* pTemporaryWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);

    if (!pTemporaryWindow)
    {
        m_err = "Could not create a temporary SDL window to get Vulkan instance extensions!";
        return false;
    }

    // Enumerate the instance extensions from SDL.
    std::vector<const char*> extensions;
    unsigned int extensionsCount = 0;
    
    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, nullptr))
    {
        SDL_DestroyWindow(pTemporaryWindow);
        m_err = "Could not get the Vulkan instance extension count from an SDL window!";
        return false;
    }

    extensions.resize(extensionsCount);
    
    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, extensions.data()))
    {
        SDL_DestroyWindow(pTemporaryWindow);
        m_err = "Could not get the Vulkan instance extensions from an SDL window!";
        return false;
    }

    // Destroy the temporary window.
    SDL_DestroyWindow(pTemporaryWindow);

    outExtensions = extensions;
    return true;
}

bool GraphicsInstance::getExtensions(std::vector<const char*>& outExtensions)
{
    
    // our engines required extensions
    std::vector<const char*> extensions = 
    {
#ifdef BLUEMETAL_DEVELOPMENT
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
    };

    // Add the SDL surface extensions to the list of extensions.
    std::vector<const char*> surfaceExtensions; 
    if (!getExtensionsForSDL(surfaceExtensions)) 
        return false;

    extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // get all the current vulkan instance extensions
    uint32_t propertiesCount = 0;
    std::vector<VkExtensionProperties> extensionProperties;
    
    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr) != VK_SUCCESS)
    {
        m_err = "Could not get Vulkan instance extensions count!";
        return false;
    }

    extensionProperties.resize(propertiesCount);

    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensionProperties.data()) != VK_SUCCESS)
    {
        m_err = "Could not enumerate Vulkan instance extension properties!";
        return false;
    }

    // check and make sure all of our extensions are present
    for (const char* pName : extensions)
    {
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), 
            [pName](const VkExtensionProperties& properties)
            {
                return strcmp(pName, properties.extensionName) == 0; 
            }) == extensionProperties.end())
        {
            m_err = fmt::format("Could not find required instance extension: {}", pName);
            return false;
        }
    }

    // the system has all our extensions
    outExtensions = extensions;
    return true;
}

bool GraphicsInstance::getValidationLayers(std::vector<const char*>& userLayers)
{
    userLayers.clear();

    // disable validation layers on release
#ifdef BLUEMETAL_DEVELOPMENT
    return true;
#else

    std::vector<const char*> layers = 
    {
        "VK_LAYER_KHRONOS_validation",  
    };

    // Get the systems validation layer info.
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr) != VK_SUCCESS)
    {
        m_err = "Could not get Vulkan instance layer properties count!";
        return false;
    }

    properties.resize(propertiesCount);

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data()) != VK_SUCCESS)
    {
        m_err = "Could not enumerate Vulkan instance layer properties!";
        return false;
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
            m_err = fmt::format("Could not find required instance layer: {}", name);
        }
    }

    // found all layers!
    userLayers = layers;
    return true;
#endif
}

bool GraphicsInstance::createInstance()
{
    std::vector<const char*> extensions;
    std::vector<const char*> layers;

    if (!getExtensions(extensions) || !getValidationLayers(layers))
        return false;

#ifdef BLUEMETAL_DEVELOPMENT
    // use a debug messenger while in development
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.pNext = nullptr;
    debugMessengerCreateInfo.flags = 0;
    debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = debugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;
#endif

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = m_createInfo.applicationName.c_str();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(m_createInfo.applicationVersion.major, m_createInfo.applicationVersion.minor, m_createInfo.applicationVersion.patch);
    applicationInfo.pEngineName = engineName.data();
    applicationInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef BLUEMETAL_DEVELOPMENT 
    createInfo.pNext = &debugMessengerCreateInfo;
#else
    createInfo.pNext = nullptr;
#endif
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledLayerCount = (uint32_t)layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        m_err = "Could not create the Vulkan instance!";
        return false;
    }

    // To get debug utils messenger the functions must be loaded in
#ifdef BLUEMETAL_DEVELOPMENT
    m_createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
    m_destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (!m_createDebugUtilsMessengerEXT || !m_destroyDebugUtilsMessengerEXT)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_err = "Could not get Vulkan debug utils messenger create and destroy pointer functions!";
        return false;
    }

    if (m_createDebugUtilsMessengerEXT(m_instance, &debugMessengerCreateInfo, nullptr, &m_messenger) != VK_SUCCESS)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_err = "Could not create a Vulkan debug utils messenger!");
        return false;
    }
#endif

    BL_LOG(LogType::eInfo, "Created Vulkan instance.")
    return true;
}

bool GraphicsInstance::createPhysicalDevices()
{

    // Enumerate all Vulkan physical devices. 
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    
    if (vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr) != VK_SUCCESS)
    {
        m_err = "Could not get the Vulkan physical device count!";
        return false;
    }

    physicalDevices.resize(physicalDeviceCount);

    if (vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS)
    {
        m_err = "Could not enumerate Vulkan physical devices!";
        return false;
    }

    // Create the bl::PhysicalDevices
    uint32_t i = 0;
    for (VkPhysicalDevice pd : physicalDevices)
    {
        m_physicalDevices.emplace_back(std::make_unique<GraphicsPhysicalDevice>(pd, i));
        i++;
    }
    
    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsInstance::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity, 
    VkDebugUtilsMessageTypeFlagsEXT             type, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
    void*                                       pUserData) noexcept
{
    (void)type;
    (void)pUserData;

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        BL_LOG(LogType::eError, "{}", pCallbackData->pMessage);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        BL_LOG(LogType::eWarning, "{}", pCallbackData->pMessage);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        BL_LOG(LogType::eInfo, "{}", pCallbackData->pMessage);
    }

    return false;
}

} // namespace bl