#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Precompiled.h"
#include "Export.h"

namespace bl
{

///////////////////////////////
// Forward Declarations
///////////////////////////////

class Window;

///////////////////////////////
// Classes
///////////////////////////////

/// A physical GPU within the system.
class BLUEMETAL_API GraphicsPhysicalDevice
{
public:

    /// Constructs a physical device object.
    ///
    ///     @param index Index for this physical device.
    ///     @param physicalDevice Vulkan physical device object.
    ///
    GraphicsPhysicalDevice(VkPhysicalDevice physicalDevice, uint32_t index);

    /// Copy Constructor
    GraphicsPhysicalDevice(GraphicsPhysicalDevice& other) = default;

    /// Move Constructor
    GraphicsPhysicalDevice(GraphicsPhysicalDevice&& other) = default;
    
    /// Default destructor.
    ~GraphicsPhysicalDevice() = default;

    /// Gets the underlying VkPhysicalDevice object.
    VkPhysicalDevice getHandle();

    /// Gets the name of the graphics card being used. (Ex. Nvidia RTX 2070)
    std::string getDeviceName();

    /// Gets the vendor name of the graphics card. (Ex. AMD, Nvidia)
    std::string getVendorName();

    /// Gets the index of this physical device as per vkEnumeratePhysicalDevices(). 
    uint32_t getIndex();

    /// Gets the available present modes on the system.
    ///
    /// Useful for @ref Swapchain::recreate.
    std::vector<VkPresentModeKHR> getPresentModes(Window* pWindow);

    /// Gets the available surface formats on this physical device.
    ///
    /// Useful for @ref Swapchain::recreate.
    std::vector<VkSurfaceFormatKHR> getSurfaceFormats(Window* pWindow);
    
    /// Finds a supported format within a list.
    ///
    ///     @param candidates List of candidates for a format, order from best to last. 
    ///     @param tiling Image tiling option wanted.
    ///     @param features Desired format features wanted.
    /// 
    ///     @return A format within the format list within candidates, VK_FORMAT_UNDEFINED if not successful.
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

private:
    VkPhysicalDevice    m_physicalDevice;
    uint32_t            m_index;
};

}