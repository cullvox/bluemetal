#pragma once

#include "Precompiled.h"
#include "Export.h"

namespace bl
{

class Window;

class BLUEMETAL_API PhysicalDevice
{

public:
    PhysicalDevice(uint32_t index, VkPhysicalDevice physicalDevice);
    ~PhysicalDevice();

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
    std::vector<VkPresentModeKHR> getPresentModes(std::shared_ptr<Window> window);

    /// Gets the available surface formats on this physical device.
    ///
    /// Useful for @ref Swapchain::recreate.
    std::vector<VkSurfaceFormatKHR> getSurfaceFormats(std::shared_ptr<Window> window);
    
    /// Finds a supported format within a list.
    ///
    ///     @param candidates List of candidates for a format, order from best to last. 
    ///     @param tiling Image tiling option wanted.
    ///     @param features Desired format features wanted.
    /// 
    ///     @return A format within the format list within candidates, VK_FORMAT_UNDEFINED if not successful.
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

private:
    uint32_t            m_index;
    VkPhysicalDevice    m_physicalDevice;

};

}