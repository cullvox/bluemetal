#include "Instance.h"
#include "Core/Print.h"
#include "Core/Version.h"

namespace bl {

GraphicsInstance::GraphicsInstance() { }

GraphicsInstance::GraphicsInstance(GraphicsInstance&& rhs)
{
    destroy();

    _createInfo = std::move(rhs._createInfo);
    _instance = std::move(rhs._instance);
    _createDebugUtilsMessengerEXT = std::move(rhs._createDebugUtilsMessengerEXT);
    _destroyDebugUtilsMessengerEXT = std::move(rhs._destroyDebugUtilsMessengerEXT);
    _messenger = std::move(rhs._messenger);
    _physicalDevices = std::move(rhs._physicalDevices);
}

GraphicsInstance& GraphicsInstance::operator=(GraphicsInstance&& rhs)
{
    destroy();

    _createInfo = std::move(rhs._createInfo);
    _instance = std::move(rhs._instance);
    _createDebugUtilsMessengerEXT = std::move(rhs._createDebugUtilsMessengerEXT);
    _destroyDebugUtilsMessengerEXT = std::move(rhs._destroyDebugUtilsMessengerEXT);
    _messenger = std::move(rhs._messenger);
    _physicalDevices = std::move(rhs._physicalDevices);

    /// Clear the values so a call to rhs.isCreated returns false.
    rhs._createInfo = {};
    rhs._instance = VK_NULL_HANDLE;
    rhs._createDebugUtilsMessengerEXT = nullptr;
    rhs._destroyDebugUtilsMessengerEXT = nullptr;
    rhs._messenger = VK_NULL_HANDLE;
    rhs._physicalDevices.clear();

    return *this;
}

GraphicsInstance::GraphicsInstance(const GraphicsInstanceCreateInfo& createInfo)
{
    if (!create(createInfo))
        throw std::runtime_error("Could not create a graphics instance! Check previous error logs!");
}

GraphicsInstance::~GraphicsInstance() { destroy(); }

bool GraphicsInstance::create(const GraphicsInstanceCreateInfo& createInfo)
{
    _createInfo = createInfo;

    return createInstance() && createPhysicalDevices();
}

void GraphicsInstance::destroy() noexcept
{
    if (!isCreated())
        return;

#ifdef BLUEMETAL_DEVELOPMENT
    _destroyDebugUtilsMessengerEXT(_instance, _messenger, nullptr);
#endif

    vkDestroyInstance(_instance, nullptr);
}

bool GraphicsInstance::isCreated() const noexcept { return _instance != VK_NULL_HANDLE; }

VkInstance GraphicsInstance::getHandle() const
{
    assert(isCreated() && "Instance must be created before a handle can be retrieved.");

    return _instance;
}

std::vector<GraphicsPhysicalDevice*> GraphicsInstance::getPhysicalDevices() const
{
    assert(isCreated() && "Instance must be created before physical devices can be retrieved.");

    // Get the raw pointer values from the unique pointer.
    std::vector<GraphicsPhysicalDevice*> physicalDevices;
    std::transform(_physicalDevices.begin(), _physicalDevices.end(),
        std::back_inserter(physicalDevices), [](auto& pd) { return pd.get(); });

    return physicalDevices;
}

bool GraphicsInstance::getExtensionsForSDL(std::vector<const char*>& outExtensions)
{

    // Create a temporary window to get extensions from SDL.
    SDL_Window* pTemporaryWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);

    if (!pTemporaryWindow) {
        blError("Could not create a temporary SDL window to get Vulkan instance extensions!");
        return false;
    }

    // Enumerate the instance extensions from SDL.
    std::vector<const char*> extensions;
    unsigned int extensionsCount = 0;

    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, nullptr)) {
        blError("Could not get the Vulkan instance extension count from an SDL window!");
        SDL_DestroyWindow(pTemporaryWindow);
        return false;
    }

    extensions.resize(extensionsCount);

    if (!SDL_Vulkan_GetInstanceExtensions(pTemporaryWindow, &extensionsCount, extensions.data())) {
        blError("Could not get the Vulkan instance extensions from an SDL window!");
        SDL_DestroyWindow(pTemporaryWindow);
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
    std::vector<const char*> extensions = {
#ifdef BLUEMETAL_DEVELOPMENT
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
    };

    // Add the SDL surface extensions to the list of extensions.
    std::vector<const char*> surfaceExtensions;
    if (!getExtensionsForSDL(surfaceExtensions)) return false;

    extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // get all the current vulkan instance extensions
    uint32_t propertiesCount = 0;
    std::vector<VkExtensionProperties> extensionProperties;

    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr) != VK_SUCCESS) {
        blError("Could not get Vulkan instance extensions count!");
        return false;
    }

    extensionProperties.resize(propertiesCount);

    if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensionProperties.data()) != VK_SUCCESS) {
        blError("Could not enumerate Vulkan instance extension properties!");
        return false;
    }

    // check and make sure all of our extensions are present
    for (const char* pName : extensions) {
        auto func = [pName](const VkExtensionProperties& properties){ return strcmp(pName, properties.extensionName) == 0; };
        if (std::find_if(extensionProperties.begin(), extensionProperties.end(), func) == extensionProperties.end()) {
            blError("Could not find required instance extension: {}", pName);
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
    
#ifdef BLUEMETAL_DEVELOPMENT
    // disable validation layers on release
    return true;
#else // BLUEMETAL_DEVELOPMENT
    std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation",
    };

    // Get the systems validation layer info.
    uint32_t propertiesCount = 0;
    std::vector<VkLayerProperties> properties;

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, nullptr) != VK_SUCCESS) {
        blError("Could not get Vulkan instance layer properties count!");
        return false;
    }

    properties.resize(propertiesCount);

    if (vkEnumerateInstanceLayerProperties(&propertiesCount, properties.data()) != VK_SUCCESS) {
        blError("Could not enumerate Vulkan instance layer properties!");
        return false;
    }

    // Ensure that the requested layers are present on the system.
    for (const char* name : layers) {
        auto func = [name](const VkLayerProperties& properties){ return strcmp(properties.layerName, name) == 0; };
        if (std::find_if(properties.begin(), properties.end(), func) == properties.end()) {
            blError("Could not find required instance layer: {}", name);
        }
    }

    // found all layers!
    userLayers = layers;
    return true;
#endif // BLUEMETAL_DEVELOPMENT
}

bool GraphicsInstance::createInstance()
{
    std::vector<const char*> extensions;
    std::vector<const char*> layers;

    if (!getExtensions(extensions) || !getValidationLayers(layers)) return false;

#ifdef BLUEMETAL_DEVELOPMENT
    // Use a debug messenger while in development.
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
    debugMessengerCreateInfo.pfnUserCallback = debugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;
#endif

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = _createInfo.applicationName.c_str();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(
        _createInfo.applicationVersion.major,
        _createInfo.applicationVersion.minor,
        _createInfo.applicationVersion.patch);
    applicationInfo.pEngineName = engineName.data();
    applicationInfo.engineVersion = VK_MAKE_VERSION(
        engineVersion.major, 
        engineVersion.minor,
        engineVersion.patch);
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

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
        blError("Could not create the Vulkan instance!");
        return false;
    }

    // To get debug utils messenger the functions must be loaded in
#ifdef BLUEMETAL_DEVELOPMENT
    _createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
    _destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

    if (!_createDebugUtilsMessengerEXT || !_destroyDebugUtilsMessengerEXT) {
        blError("Could not get Vulkan debug utils messenger create and destroy pointer functions!");
        vkDestroyInstance(_instance, nullptr);
        return false;
    }

    if (_createDebugUtilsMessengerEXT(_instance, &debugMessengerCreateInfo, nullptr, &_messenger) != VK_SUCCESS) {
        blError("Could not create a Vulkan debug utils messenger!");
        vkDestroyInstance(_instance, nullptr);
        return false;
    }
#endif

    blInfo("Vulkan instance created.");
    return true;
}

bool GraphicsInstance::createPhysicalDevices()
{

    // Enumerate all Vulkan physical devices.
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;

    if (vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr) != VK_SUCCESS) {
        blError("Could not get the Vulkan physical device count!");
        return false;
    }

    physicalDevices.resize(physicalDeviceCount);

    if (vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS) {
        blError("Could not enumerate Vulkan physical devices!");
        return false;
    }

    // Create the bl::PhysicalDevices
    uint32_t i = 0;
    for (VkPhysicalDevice pd : physicalDevices) {
        _physicalDevices.emplace_back(pd, i);
        i++;
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsInstance::debugCallback(
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
