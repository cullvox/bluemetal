#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Core/Version.h"
#include "PhysicalDevice.h"
#include "Precompiled.h"
#include "Export.h"

namespace bl
{

struct GraphicsInstanceCreateInfo
{
    Version         applicationVersion;
    std::string     applicationName;
    bool            enableValidation;
};

class BLUEMETAL_API GraphicsInstance
{
public:

    /// Default Constructor
    GraphicsInstance();

    /// Move Constructor 
    GraphicsInstance(GraphicsInstance&& other);

    /// Create Constructor
    GraphicsInstance(const GraphicsInstanceCreateInfo& createInfo);
    
    /// Default Destructor
    ~GraphicsInstance();

    /// Move Operator
    GraphicsInstance& operator=(GraphicsInstance&& rhs);

    /// Creates the instance object.
    bool create(const GraphicsInstanceCreateInfo& createInfo);

    /// Destroys the instance object.
    void destroy() noexcept;

    /// Returns true if this object was created.
    bool isCreated() const noexcept;

    /// Returns true if this object was created.
    ///
    ///     @param[out] error Error string to set if there was an issue.
    bool isCreated(std::string& error) const noexcept;

    /// Gets the create info used to create this object.
    ///
    ///     @param[out] createInfo Create info struct to fill in.
    ///
    bool getCreateInfo(GraphicsInstanceCreateInfo& createInfo);
    
    /// Gets the underlying VkInstance object.
    VkInstance getHandle() const;
 
    /// Gets the possible physical devices to choose from.
    std::vector<GraphicsPhysicalDevice*> getPhysicalDevices() const;

private:
 
    /// Gets a list of extensions used by SDL for window creation. 
    bool getExtensionsForSDL(std::vector<const char*>& layers);

    /// Gets a list of extensions used by this engine for various usages. 
    bool getExtensions(std::vector<const char*>& layers);

    /// Gets a list of validation layers used by this engine. 
    bool getValidationLayers(std::vector<const char*>& layers);

    /// Creates the underlying VkInstance object.
    bool createInstance();

    /// Creates the array of GraphicsPhysicalDevices from VkPhysicalDevices.
    bool createPhysicalDevices();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT          severity,
        VkDebugUtilsMessageTypeFlagsEXT                 messageType,
        const VkDebugUtilsMessengerCallbackDataEXT*     pCallbackData,
        void*                                           pUserData) noexcept;
    
    std::string                                             m_err;
    GraphicsInstanceCreateInfo                              m_createInfo;
    VkInstance                                              m_instance;
    PFN_vkCreateDebugUtilsMessengerEXT                      m_createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT                     m_destroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT                                m_messenger;
    std::vector<std::unique_ptr<GraphicsPhysicalDevice>>    m_physicalDevices;
};

} // namespace bl