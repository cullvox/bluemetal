#include "Instance.h"
#include "Version.h"
#include "Core/Log.h"

namespace bl
{

Instance::Instance()
{
    createInstance();
}

Instance::~Instance()
{
    m_destroyDebugUtilsMessengerEXT(m_instance, m_messenger, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

std::vector<const char*> Instance::getExtensionsForSDL()
{

    // create a temporary window to get extensions from SDL
    SDL_Window* pTemporaryWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);

    if (!pTemporaryWindow)
    {
        throw std::runtime_error("Could not create a temporary SDL window to get Vulkan instance extensions!");
    }

    // enumerate the instance extensions from SDL
    std::vector<const char*> extensions;
    unsigned int extensionsCount = 0;
    
    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, nullptr))
    {
        SDL_DestroyWindow(pTemporaryWindow);
        throw std::runtime_error("Could not get the Vulkan instance extension count from an SDL window!");
    }

    extensions.resize(extensionsCount);
    
    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, extensions.data()))
    {
        SDL_DestroyWindow(pTemporaryWindow);
        throw std::runtime_error("Could not get the Vulkan instance extensions from an SDL window!");
    }

    // destroy temp window
    SDL_DestroyWindow(pTemporaryWindow);

    return extensions;
}

std::vector<const char*> Instance::getExtensions()
{
    
    // our engines required extensions
    std::vector<const char*> extensions = 
    {
#ifdef BLUEMETAL_DEVELOPMENT
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
    };

    // add the surface extensions to the list of extensions
    std::vector<const char*> surfaceExtensions = getExtensionsForSDL();
    extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // get all the current vulkan instance extensions
    uint32_t propertiesCount = 0;
    std::vector<VkExtensionProperties> extensionProperties;
    
    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan instance extensions count!");
    }

    extensionProperties.resize(propertiesCount);

    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensionProperties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate Vulkan instance extension properties!");
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
            BL_LOG(LogType::eFatal, "Could not find required instance extension: {}", pName);
        }
    }

    // the system has all our extensions
    return extensions;
}

std::vector<const char*> Instance::getValidationLayers()
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

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan instance layer properties count!");
    }

    properties.resize(propertiesCount);

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate Vulkan instance layer properties!");
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
            throw std::runtime_error(fmt::format("Could not find required instance layer: {}", name));
        }
    }

    // found all layers!
    return layers;
}

void Instance::createInstance()
{
    std::vector<const char*> instanceExtensions = getExtensions();
    std::vector<const char*> validationLayers = getValidationLayers();

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

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = applicationName.data();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(applicationVersion.major, applicationVersion.minor, applicationVersion.patch);
    applicationInfo.pEngineName = engineName.data();
    applicationInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
    createInfo.pNext = BLUEMETAL_DEVELOPMENT ? &debugMessengerCreateInfo : nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan instance!");
    }

    // To get debug utils messenger the functions must be loaded in
    if (BLUEMETAL_DEVELOPMENT)
    {
        m_createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
        m_destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
        
        if (!m_createDebugUtilsMessengerEXT || !m_destroyDebugUtilsMessengerEXT)
        {
            vkDestroyInstance(m_instance, nullptr);
            throw std::runtime_error("Could not get Vulkan debug utils messenger create and destroy pointer functions!");
        }

        if (m_createDebugUtilsMessengerEXT(m_instance, &debugMessengerCreateInfo, nullptr, &m_messenger) != VK_SUCCESS)
        {
            vkDestroyInstance(m_instance, nullptr);
            throw std::runtime_error("Could not create a Vulkan debug utils messenger!");
        }
    }

    BL_LOG(LogType::eInfo, "vulkan instance created")
}

void Instance::createPhysicalDevices()
{

    // Enumerate all Vulkan physical devices. 
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    
    if (vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get the Vulkan physical device count!");
    }

    physicalDevices.resize(physicalDeviceCount);

    if (vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate Vulkan physical devices!");
    }

    // Create the bl::PhysicalDevices
    uint32_t i = 0;
    for (VkPhysicalDevice pd : physicalDevices)
    {
        m_physicalDevices.push_back(std::make_shared<PhysicalDevice>(i, pd));
        i++;
    }   
}

} // namespace bl