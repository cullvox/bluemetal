#include "Instance.h"
#include "Core/Print.h"
#include "Core/Version.h"
#include "Window/Window.h"

namespace bl {

GfxInstance::GfxInstance(
    Version             appVersion,
    std::string_view    appName,
    bool                enableValidation)
    : _enableValidation(enableValidation)
{
    createInstance(appVersion, appName);
    enumeratePhysicalDevices();
}

GfxInstance::~GfxInstance()
{
    if (!_instance) return;

    if (_enableValidation)
        _destroyDebugUtilsMessengerEXT(_instance, _messenger, nullptr);
    
    vkDestroyInstance(_instance, nullptr);
    volkFinalize();
}

bool GfxInstance::getEnableValidation() const
{
    return _enableValidation;
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

    // Enumerate the instance extensions from SDL.
    std::vector<const char*> extensions;
    unsigned int extensionsCount = 0;

    Window::useTemporaryWindow([&extensions, &extensionsCount](SDL_Window* window){    
        SDL_Vulkan_GetInstanceExtensions(window, &extensionsCount, nullptr);
        extensions.resize(extensionsCount);
        SDL_Vulkan_GetInstanceExtensions(window, &extensionsCount, extensions.data());
    });

    return extensions;
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

    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr));
    extensionProperties.resize(propertiesCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensionProperties.data()));

    // Make sure all the extensions wanted are present.
    for (const char* pName : extensions) {
        auto func = [pName](const VkExtensionProperties& properties){ return strcmp(pName, properties.extensionName) == 0; };
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), func) == extensionProperties.end()) {
            throw std::runtime_error("Could not find a required instance extension!");
        }
    }

    return extensions;
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

    VK_CHECK(vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr));
    properties.resize(propertiesCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data()));

    // Ensure that the requested layers are present on the system.
    for (const char* name : layers) {
        auto func = [name](const VkLayerProperties& properties){ return strcmp(properties.layerName, name) == 0; };
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            throw std::runtime_error("Could not find a required instance layer!");
        }
    }

    return layers;
}

void GfxInstance::createInstance(
    Version             appVersion,
    std::string_view    appName)
{
    volkInitialize();

    std::vector<const char*> extensions = getExtensions();
    std::vector<const char*> layers{};

    if (_enableValidation)
        layers = getValidationLayers();


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
    applicationInfo.pApplicationName = appName.data();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
    applicationInfo.pEngineName = engineName.data();
    applicationInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
    applicationInfo.apiVersion = GfxInstance::getApiVersion();

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = _enableValidation ? &debugMessengerCreateInfo : nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = (uint32_t)layers.size();
    instanceCreateInfo.ppEnabledLayerNames = layers.data();
    instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &_instance));

    // To enable validation the debug utils messenger functions must be loaded in.
    if (_enableValidation) {
        _createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
        _destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

        if (!_createDebugUtilsMessengerEXT || !_destroyDebugUtilsMessengerEXT) {
            throw std::runtime_error("Could not get Vulkan debug utils messenger create and destroy pointer functions!");
        }

        VK_CHECK(_createDebugUtilsMessengerEXT(_instance, &debugMessengerCreateInfo, nullptr, &_messenger));
    }

    volkLoadInstance(_instance);
    blDebug("Finshed creating the Vulkan instance.");
}

void GfxInstance::enumeratePhysicalDevices()
{

    // Enumerate all Vulkan physical devices.
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;

    VK_CHECK(vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr));
    physicalDevices.resize(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data()))

    // Create the bl::PhysicalDevices
    uint32_t i = 0;
    for (VkPhysicalDevice pd : physicalDevices) {
        _physicalDevices.push_back(std::make_shared<GfxPhysicalDevice>(pd, i));
        i++;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL GfxInstance::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT         severity, 
    VkDebugUtilsMessageTypeFlagsEXT                /* type */,
    const VkDebugUtilsMessengerCallbackDataEXT*    pCallbackData, 
    void*                                          /* pUserData */) noexcept
{
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
