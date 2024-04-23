#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "Core/Version.h"
#include "Vulkan.h"
#include "Config.h"
#include "PhysicalDevice.h"

namespace bl 
{

/** @brief Used to choose physical device using an instance to create an @ref GfxDevice. */
class Instance : public NonCopyable
{
public:
    Instance(Version appVersion, std::string_view appName, bool enableValidation); /** @brief Constructor */
    ~Instance(); /** @brief Destructor*/

    VkInstance Get() const; /** @brief Returns the underlying VkInstance object. */
    bool GetValidationEnabled() const; /** @brief Returns true if validation layers are enabled. */
    const std::vector<PhysicalDevice>& GetPhysicalDevices() const; /** @brief Returns the physical devices to choose from. */    
    PhysicalDevice& ChoosePhysicalDevice() const; /** @brief Magically chooses a physical device that might be a good choice to use for device creation. */

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData); /** @brief Vulkan debug callback from VkDebugUtilsMessengerEXT. */
    std::vector<const char*> GetExtensionsForSDL(); /** @brief Gets a list of extensions used by SDL for window creation.  */
    std::vector<const char*> GetExtensions(); /** @brief Gets a list of extensions used by this engine for various usages.  */
    std::vector<const char*> GetValidationLayers(); /** @brief Gets a list of validation layers used by this engine.  */
    void CreateInstance(Version appVersion, std::string_view appName); /** @brief Creates the underlying VkInstance object. */
    void EnumeratePhysicalDevices(); /** @brief Creates the array of GraphicsPhysicalDevices from VkPhysicalDevices. */

    VkInstance _instance;
    bool _enableValidation;
    PFN_vkCreateDebugUtilsMessengerEXT _createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT _destroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT _messenger;
    std::vector<PhysicalDevice> _physicalDevices;
};

} // namespace bl
