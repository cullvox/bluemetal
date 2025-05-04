#include "Core/Print.h"
#include "Window/Window.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"

namespace bl {

VulkanInstance::VulkanInstance(Version appVersion, std::string_view appName, bool enableValidation)
    : _enableValidation(enableValidation) 
{
    CreateInstance(appVersion, appName);
    EnumeratePhysicalDevices();
}

VulkanInstance::~VulkanInstance() 
{
    if (_enableValidation)
        vkDestroyDebugUtilsMessengerEXT(_instance, _messenger, nullptr);

    vkDestroyInstance(_instance, nullptr);
    volkFinalize();
}

VkInstance VulkanInstance::Get() const 
{
    return _instance;
}

bool VulkanInstance::GetValidationEnabled() const 
{
    return _enableValidation;
}

std::vector<const VulkanPhysicalDevice*> VulkanInstance::GetPhysicalDevices() const 
{
    std::vector<const VulkanPhysicalDevice*> out;
    for (const auto& pd : _physicalDevices)
        out.push_back(&pd);
    
    return out;
}

const VulkanPhysicalDevice* VulkanInstance::ChoosePhysicalDevice() const 
{
    auto physicalDevices = GetPhysicalDevices();
    auto it = std::find_if(physicalDevices.begin(), physicalDevices.end(), 
        [](const auto& pd)
        { 
            return pd->GetType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; 
        });
                    
    if (it != physicalDevices.end()) { 
        return *it;
    } else {
        blWarning("Using physical device zero because user does not have a discrete card!");
        return physicalDevices[0];
    } 
}

std::vector<const char*> VulkanInstance::GetExtensions() 
{
    // Add required instance extensions below...
    std::vector<const char*> extensions = 
    {
#ifdef BLUEMETAL_VULKAN_PORTABILITY
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#endif
    };

    if (_enableValidation) // Validation only required when enabled
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Add the SDL surface extensions to the list of extensions.
    auto surfaceExtensions = Window::GetVulkanExtensions();
    extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());

    // Get all the current vulkan instance extensions.
    uint32_t propertiesCount = 0;
    std::vector<VkExtensionProperties> extensionProperties;

    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr));
    extensionProperties.resize(propertiesCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, extensionProperties.data()));

    // Make sure all the extensions wanted are present.
    for (const char* name : extensions) 
    {
        if (!std::any_of(extensionProperties.begin(), extensionProperties.end(), 
            [name](const auto& properties){ 
                return strcmp(name, properties.extensionName) == 0; 
            })) 
        {
            throw std::runtime_error("Could not find a required instance extension!");
        }
    }

    return extensions;
}

std::vector<const char*> VulkanInstance::GetValidationLayers()
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
        if (!std::any_of(properties.begin(), properties.end(), 
            [name](const auto& properties){ 
                return strcmp(name, properties.layerName) == 0; 
            })) {
            blError("User requested validation layers but none were found! Is the Vulkan SDK installed?");
            _enableValidation = false;
        }
    }

    return layers;
}

void VulkanInstance::CreateInstance(Version appVersion, std::string_view appName)
{
    volkInitialize();

    std::vector<const char*> extensions = GetExtensions();
    std::vector<const char*> layers{};

    if (_enableValidation) layers = GetValidationLayers();

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.pNext = nullptr;
    debugMessengerCreateInfo.flags = 0;
    debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = VulkanInstance::DebugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = appName.data();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
    applicationInfo.pEngineName = engineName.data();
    applicationInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
    applicationInfo.apiVersion = VulkanConfig::apiVersion;

    VkInstanceCreateFlags flags = 0;
    #ifdef BLUEMETAL_VULKAN_PORTABILITY
    flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = _enableValidation ? &debugMessengerCreateInfo : nullptr;
    instanceCreateInfo.flags = flags;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = (uint32_t)layers.size();
    instanceCreateInfo.ppEnabledLayerNames = layers.data();
    instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &_instance))

    volkLoadInstance(_instance); // I wish that volk was a little better encapsulated here, but since it's a function loader not much we can do.

    // To enable validation the debug utils messenger functions must be loaded in.
    if (_enableValidation)
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(_instance, &debugMessengerCreateInfo, nullptr, &_messenger))

    blDebug("Finshed creating the Vulkan instance.");
}

void VulkanInstance::EnumeratePhysicalDevices() {
    uint32_t physicalDeviceCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices{};

    VK_CHECK(vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr));
    physicalDevices.resize(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, physicalDevices.data()))

    _physicalDevices.reserve(physicalDeviceCount);
    for (VkPhysicalDevice pd : physicalDevices)
        _physicalDevices.emplace_back(pd);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
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
