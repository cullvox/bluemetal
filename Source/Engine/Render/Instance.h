#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "PhysicalDevice.h"

namespace bl
{


class BLUEMETAL_API Instance
{
public:

    /// Default Constructor.
    Instance();
    
    /// @brief Default Destructor.
    ~Instance();

    /// Gets the underlying VkInstance object.
    VkInstance getHandle();
 
    /// Gets the underlying VkPhysicalDevice representing the choosen GPU.
    std::vector<std::shared_ptr<PhysicalDevice>> getPhysicalDevices();

private:
 
    /// Gets a list of extensions used by SDL for window creation. 
    std::vector<const char*> getExtensionsForSDL();

    /// Gets a list of extensions used by this engine for various usages. 
    std::vector<const char*> getExtensions();

    /// Gets a list of validation layers used by this engine. 
    std::vector<const char*> getValidationLayers();

    /// Creates the underlying VkInstance object.
    void createInstance();

    /// Creates the array of bl::PhysicalDevices from VkPhysicalDevices.
    void createPhysicalDevices();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,VkDebugUtilsMessageTypeFlagsEXT,const VkDebugUtilsMessengerCallbackDataEXT*,void*) noexcept;
    
    VkInstance                                      m_instance;
    PFN_vkCreateDebugUtilsMessengerEXT              m_createDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT             m_destroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT                        m_messenger;
    std::vector<std::shared_ptr<PhysicalDevice>>    m_physicalDevices;
};

} // namespace bl