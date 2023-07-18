#include "Instance.h"
#include "Version.h"
#include "Core/Log.h"

blInstance::blInstance(std::optional<uint32_t> physicalDeviceIndex)
{
    createInstance();
    choosePhysicalDevice(physicalDeviceIndex);
}

std::vector<const char*> blInstance::getExtensionsForSDL()
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

std::vector<const char*> blInstance::getExtensions()
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
            BL_LOG(blLogType::eFatal, "Could not find required instance extension: {}", pName);
        }
    }

    // the system has all our extensions
    return extensions;
}

std::vector<const char*> blInstance::getValidationLayers()
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

void blInstance::createInstance()
{
    std::vector<const char*> instanceExtensions = getExtensions();
    std::vector<const char*> validationLayers = getValidationLayers();

    // use a debug messenger while in development
    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo
    {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,    // sType
        nullptr,                                                    // pNext
        0,                                                          // flags
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |           // messageSeverity
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |           // messageType
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        debugCallback,                                              // pfnUserCallback
        nullptr                                                     // pUserData
    };

    const VkApplicationInfo applicationInfo
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
        nullptr,                            // pNext
        applicationName.data(),             // pApplicationName
        VK_MAKE_VERSION(                    // applicationVersion
            applicationVersion.major,
            applicationVersion.minor,
            applicationVersion.patch),      
        engineName.data(),                  // pEngineName
        VK_MAKE_VERSION(                    // engineVersion
            engineVersion.major, 
            engineVersion.minor, 
            engineVersion.patch),        
        VK_API_VERSION_1_2,                 // apiVersion
    };

    const VkInstanceCreateInfo createInfo
    {    
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,     // sType
#ifdef BLUEMETAL_DEVELOPMENT                        // pNext
        &debugMessengerCreateInfo,
#else
        nullptr
#endif
        0,                                          // flags
        &applicationInfo,                           // pApplicationInfo
        (uint32_t)validationLayers.size(),          // enabledLayerCount
        validationLayers.data(),                    // pEnabledLayerNames
        (uint32_t)instanceExtensions.size(),        // enabledExtensionCount
        instanceExtensions.data(),                  // pEnabledExtensionNames
    };

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan instance!");
    }

    BL_LOG(blLogType::eInfo, "vulkan instance created")
}

void blInstance::choosePhysicalDevice(std::optional<uint32_t> physicalDeviceIndex)
{
    // get the vulkan physical devices
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    if (vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not get Vulkan physical device count!");
    }

    physicalDevices.resize(physicalDeviceCount);

    if (vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not enumerate Vulkan physical devices!");
    }

    // check if we want a specific device
    if (physicalDeviceIndex)
    {
        if (physicalDeviceIndex.value() > physicalDeviceCount - 1)
        {
            throw std::runtime_error("Choosen physical device index is out of range!");
        }

        _physicalDevice = physicalDevices[physicalDeviceIndex.value()];
    }
    else
    {
        BL_LOG(blLogType::eInfo, "Physical device not choosen, using default.");
        _physicalDevice = physicalDevices[0];
    }

    // log the device's name
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    BL_LOG(blLogType::eInfo, "Choose physical device: {}", properties.deviceName);
}


std::string blInstance::getVendorName()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    switch (properties.vendorID)
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

std::string blInstance::getDeviceName()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

    return std::string(properties.deviceName);
}

VkFormat blInstance::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties{};
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Could not find any valid format!");
}