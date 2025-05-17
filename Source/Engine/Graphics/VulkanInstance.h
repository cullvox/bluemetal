#pragma once

#include "Precompiled.h"
#include "Core/Version.h"
#include "Vulkan.h"
#include "VulkanConfig.h"
#include "VulkanPhysicalDevice.h"
#include "Window/WindowPlatform.h"

namespace bl 
{

/// @brief Vulkan instance object, used to create a vulkan device and choose physical devices.
class VulkanInstance : public NonCopyable 
{
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
    
    /// @brief Destructor
    ~VulkanInstance();

    VulkanInstance& operator=(VulkanInstance&& move) noexcept;

    /// @brief Returns the underlying VkInstance object.
    VkInstance Get() const;

    /// @brief Returns true if validation layers were used when creating this instance.
    bool GetValidationEnabled() const;

    /// @brief Returns all physical devices available to choose from.
    std::vector<VulkanPhysicalDevice*> GetPhysicalDevices();

    /// @brief Chooses a physical device automagically and returns the best candidate for device creation. 
    VulkanPhysicalDevice* ChoosePhysicalDevice();

private:
    /// @brief Vulkan debug callback from VkDebugUtilsMessengerEXT.
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    /// @brief Gets a list of extensions used by this engine for various usages.
    std::vector<const char*> GetExtensions();

    /// @brief Gets a list of validation layers used by this engine.
    std::vector<const char*> GetValidationLayers();

    /// @brief Creates the underlying VkInstance object.
    void CreateInstance(Version appVersion, std::string_view appName);

    /// @brief Creates the array of GraphicsPhysicalDevices from VkPhysicalDevices.
    void EnumeratePhysicalDevices();

    VkInstance _instance;
    bool _enableValidation;
    VkDebugUtilsMessengerEXT _messenger;
    std::vector<VulkanPhysicalDevice> _physicalDevices;
};

} // namespace bl
