#include "Instance.h"
#include "Core/Print.h"
#include "Core/Version.h"

namespace bl {

GfxInstance::GfxInstance(const CreateInfo& createInfo) 
{
    createInstance(createInfo);
    enumeratePhysicalDevices();
}

GfxInstance::~GfxInstance()
{
    if (!_instance) return;

    if (_enableValidation)
        _destroyDebugUtilsMessengerEXT(_instance, _messenger, nullptr);
    
    vkDestroyInstance(_instance, nullptr);
}

VkInstance GfxInstance::get() const
{
    return _instance;
}

std::vector<std::shared_ptr<GfxPhysicalDevice>> GfxInstance::getPhysicalDevices() const
{
    return _physicalDevices;
}

std::vector<const char*> GfxInstance::getExtensionsForSDL()
{

    // Create a temporary window to get extensions from SDL.
    SDL_Window* pTemporaryWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);

    if (!pTemporaryWindow) {
        throw std::runtime_error("Could not create a temporary SDL window to get Vulkan instance extensions!");
    }

    // Enumerate the instance extensions from SDL.
    std::vector<const char*> extensions;
    unsigned int extensionsCount = 0;

    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, nullptr)) {
        SDL_DestroyWindow(pTemporaryWindow);
        throw std::runtime_error("Could not get the Vulkan instance extension count from an SDL window!");
    }

    extensions.resize(extensionsCount);

    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, extensions.data())) {
        SDL_DestroyWindow(pTemporaryWindow);
        throw std::runtime_error("Could not get the Vulkan instance extensions from an SDL window!");
    }

    // Destroy the temporary window.
    SDL_DestroyWindow(pTemporaryWindow);

    return std::move(extensions);
}

std::vector<const char*> GfxInstance::getExtensions()
{

    // Add required extensions below...
    std::vector<const char*> extensions{};

    if (_enableValidation) // Validation only required when enabled
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Add the SDL surface extensions to the list of extensions.
    std::vector<const char*> surfaceExtensions = getExtensionsForSDL();
    extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // get all the current vulkan instance extensions
    uint32_t propertiesCount = 0;
    std::vector<VkExtensionProperties> extensionProperties;

    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan instance extensions count!");
    }

    extensionProperties.resize(propertiesCount);

    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensionProperties.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not enumerate Vulkan instance extension properties!");
    }

    // Make sure all the extensions wanted are present.
    for (const char* pName : extensions) {
        auto func = [pName](const VkExtensionProperties& properties){ return strcmp(pName, properties.extensionName) == 0; };
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), func) == extensionProperties.end()) {
            throw std::runtime_error("Could not find a required instance extension!");
        }
    }

    return std::move(extensions);
}

std::vector<const char*> GfxInstance::getValidationLayers()
{
    
    // The engines required validation layers.
    std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    // Get the systems validation layer info.
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get Vulkan instance layer properties count!");
    }

    properties.resize(propertiesCount);

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not enumerate Vulkan instance layer properties!");
    }

    // Ensure that the requested layers are present on the system.
    for (const char* name : layers) {
        auto func = [name](const VkLayerProperties& properties){ return strcmp(properties.layerName, name) == 0; };
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            throw std::runtime_error("Could not find a required instance layer!");
        }
    }

    return std::move(layers);
}

void GfxInstance::createInstance(const CreateInfo& createInfo)
{
    const std::vector<const char*> extensions = std::move(getExtensions());
    const std::vector<const char*> layers = std::move(getValidationLayers());

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.pNext = nullptr;
    debugMessengerCreateInfo.flags = 0;
    debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = DebugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = createInfo.applicationName.data();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(
        createInfo.applicationVersion.major,
        createInfo.applicationVersion.minor,
        createInfo.applicationVersion.patch);
    applicationInfo.pEngineName = engineName.data();
    applicationInfo.engineVersion = VK_MAKE_VERSION(
        engineVersion.major, 
        engineVersion.minor,
        engineVersion.patch);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = _enableValidation ? &debugMessengerCreateInfo : nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = (uint32_t)layers.size();
    instanceCreateInfo.ppEnabledLayerNames = layers.data();
    instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("Could not create the Vulkan instance!");
    }

    // To enable validation the debug utils messenger functions must be loaded in.
    if (_enableValidation) {
        _createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
        _destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

        if (!_createDebugUtilsMessengerEXT || !_destroyDebugUtilsMessengerEXT) {
            throw std::runtime_error("Could not get Vulkan debug utils messenger create and destroy pointer functions!");
        }

        if (_createDebugUtilsMessengerEXT(_instance, &debugMessengerCreateInfo, nullptr, &_messenger) != VK_SUCCESS) {
            throw std::runtime_error("Could not create a Vulkan debug utils messenger!");
        }
    }

    blDebug("Finshed creating the Vulkan instance.");
}

void GfxInstance::enumeratePhysicalDevices()
{

    // Enumerate all Vulkan physical devices.
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;

    if (vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Could not get the Vulkan physical device count!");
    }

    physicalDevices.resize(physicalDeviceCount);

    if (vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not enumerate Vulkan physical devices!");
    }

    // Create the bl::PhysicalDevices
    uint32_t i = 0;
    for (VkPhysicalDevice pd : physicalDevices) {
        _physicalDevices.push_back(std::make_shared<GfxPhysicalDevice>(pd, i));
        i++;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL GfxInstance::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT         severity, 
    VkDebugUtilsMessageTypeFlagsEXT                type,
    const VkDebugUtilsMessengerCallbackDataEXT*    pCallbackData, 
    void*                                          pUserData) noexcept
{
    (void)type;
    (void)pUserData;

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        blError("{}", pCallbackData->pMessage);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        blWarning("{}", pCallbackData->pMessage);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        blInfo("{}", pCallbackData->pMessage);
    }

    return false;
}

} // namespace bl
