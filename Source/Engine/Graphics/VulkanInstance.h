#pragma once

#include "Precompiled.h"
#include "Core/Version.h"
#include "Vulkan.h"
#include "VulkanConfig.h"
#include "VulkanPhysicalDevice.h"

namespace bl 
{

/** @brief Used to choose physical device using an instance to create an @ref GfxDevice. */
class VulkanInstance : public NonCopyable {
public:

    /// @brief Default Constructor
    VulkanInstance();

    /// @brief Move Constructor
    /// @param[inout] instance The source instance to move all values from.
    VulkanInstance(VulkanInstance&& instance);

    /// @brief Instance Constructor
    /// @param appVersion The version of your application.
    /// @param appName The name of your application.
    /// @param enableValidation Enables Vulkan validation layers on your device.
    VulkanInstance(Version appVersion, std::string_view appName, bool enableValidation);
    ~VulkanInstance(); /** @brief Destructor*/

    VkInstance Get() const; /** @brief Returns the underlying VkInstance object. */
    bool GetValidationEnabled() const; /** @brief Returns true if validation layers are enabled. */
    std::vector<const VulkanPhysicalDevice*> GetPhysicalDevices() const; /** @brief Returns the physical devices to choose from. */    
    const VulkanPhysicalDevice* ChoosePhysicalDevice() const; /** @brief Magically chooses a physical device that might be a good choice to use for device creation. */

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
    std::vector<VulkanPhysicalDevice> _physicalDevices;
};

} // namespace bl
