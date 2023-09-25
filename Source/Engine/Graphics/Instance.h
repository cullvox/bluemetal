#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "Core/Version.h"
#include "PhysicalDevice.h"

namespace bl {

struct GraphicsInstanceCreateInfo {
    Version         applicationVersion; /** @brief Version of your application. */
    std::string     applicationName;    /** @brief Name of your application. */
    bool            enableValidation;   /** @brief Enables Vulkan validation layer and debug logging. */
};

/** @brief Used to choose physical device using an instance to create an @ref GraphicsDevice. */
class BLUEMETAL_API GraphicsInstance {
public:
    /** @brief Default Constructor */
    GraphicsInstance();

    /** @brief Move Constructor  */
    GraphicsInstance(GraphicsInstance&& rhs) noexcept;

    /** @brief Create Constructor */
    GraphicsInstance(const GraphicsInstanceCreateInfo& info);
    
    /** @brief Default Destructor */
    ~GraphicsInstance();

    /** @brief Move Operator */
    GraphicsInstance& operator=(GraphicsInstance&& rhs) noexcept;

    /** @brief Creates this instance object. */
    [[nodiscard]] bool create(const GraphicsInstanceCreateInfo& info) noexcept;

    /** @brief Destroys this instance object. */
    void destroy() noexcept;

    /** @brief Returns true if this object was created. */
    [[nodiscard]] bool isCreated() const noexcept;

public:
    
    /** @brief Returns the underlying VkInstance object. */
    [[nodiscard]] VkInstance getHandle() const noexcept;
 
    /** @brief Returns the physical devices to choose from. */
    [[nodiscard]] std::vector<GraphicsPhysicalDevice*> getPhysicalDevices() const noexcept;

private:
 
    /** @brief Gets a list of extensions used by SDL for window creation.  */
    [[nodiscard]] bool getExtensionsForSDL(std::vector<const char*>& layers) noexcept;

    /** @brief Gets a list of extensions used by this engine for various usages.  */
    [[nodiscard]] bool getExtensions(std::vector<const char*>& layers) noexcept;

    /** @brief Gets a list of validation layers used by this engine.  */
    [[nodiscard]] bool getValidationLayers(std::vector<const char*>& layers) noexcept;

    /** @brief Creates the underlying VkInstance object. */
    [[nodiscard]] bool createInstance() noexcept;

    /** @brief Creates the array of GraphicsPhysicalDevices from VkPhysicalDevices. */
    [[nodiscard]] bool createPhysicalDevices() noexcept;

    /** @brief Vulkan debug callback from VkDebugUtilsMessengerEXT. */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT          severity,
        VkDebugUtilsMessageTypeFlagsEXT                 messageType,
        const VkDebugUtilsMessengerCallbackDataEXT*     pCallbackData,
        void*                                           pUserData) noexcept;
    
    GraphicsInstanceCreateInfo                              _createInfo;
    VkInstance                                              _instance;
    PFN_vkCreateDebugUtilsMessengerEXT                      _createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                     _destroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT                                _messenger;
    std::vector<std::unique_ptr<GraphicsPhysicalDevice>>    _physicalDevices;
};

} // namespace bl
