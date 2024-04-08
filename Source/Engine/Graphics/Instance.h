#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "Core/Version.h"
#include "Vulkan.h"
#include "PhysicalDevice.h"

namespace bl {

/** @brief Used to choose physical device using an instance to create an @ref GfxDevice. */
class BLUEMETAL_API GfxInstance 
{
public:
    
    /** @brief Specifies what version of Vulkan we are using. */
    static constexpr uint32_t getApiVersion() { return VK_API_VERSION_1_3; }

public:

    /** @brief Creates this instance object. */
    GfxInstance(        
        Version             appVersion,         /** @brief Version of your application. */
        std::string_view    appName,            /** @brief Name of your application. */
        bool                enableValidation);  /** @brief Enables/disables Vulkan validation layer and debug logging. */

    /** @brief Destroys this instance object. */
    ~GfxInstance();

public:

    /** @brief Returns the physical devices to choose from. */
    std::vector<std::shared_ptr<GfxPhysicalDevice>> getPhysicalDevices() const;

    /** @brief Returns the underlying VkInstance object. */
    VkInstance get() const;

private:

    /** @brief Gets a list of extensions used by SDL for window creation.  */
    std::vector<const char*> getExtensionsForSDL();

    /** @brief Gets a list of extensions used by this engine for various usages.  */
    std::vector<const char*> getExtensions();

    /** @brief Gets a list of validation layers used by this engine.  */
    std::vector<const char*> getValidationLayers();

    /** @brief Creates the underlying VkInstance object. */
    void createInstance(
        Version             appVersion,
        std::string_view    appName);

    /** @brief Creates the array of GraphicsPhysicalDevices from VkPhysicalDevices. */
    void enumeratePhysicalDevices();

    /** @brief Vulkan debug callback from VkDebugUtilsMessengerEXT. */
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT          severity,
        VkDebugUtilsMessageTypeFlagsEXT                 messageType,
        const VkDebugUtilsMessengerCallbackDataEXT*     pCallbackData,
        void*                                           pUserData) noexcept;
    
    VkInstance                                      _instance;
    bool                                            _enableValidation;
    PFN_vkCreateDebugUtilsMessengerEXT              _createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT             _destroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT                        _messenger;
    std::vector<std::shared_ptr<GfxPhysicalDevice>> _physicalDevices;
};

} // namespace bl
