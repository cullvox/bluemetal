#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "Vulkan.h"

namespace bl
{

class RenderWindow;

/** @brief A physical GPU within the system. */
class BLUEMETAL_API GfxPhysicalDevice
{
public:

    /** @brief Constructor */
    GfxPhysicalDevice(VkPhysicalDevice physicalDevice, uint32_t index);

    /** @brief Destructor */
    ~GfxPhysicalDevice();

    /** @brief Move Operator */ 
    GfxPhysicalDevice& operator=(GfxPhysicalDevice&& rhs) = default;
    GfxPhysicalDevice& operator=(GfxPhysicalDevice& rhs) = delete; /* No Copy */

    /** @brief Gets the underlying VkPhysicalDevice object. */ 
    VkPhysicalDevice get() const;

    /** @brief Returns the type of physical devices this is. (Integrated, External... )*/
    VkPhysicalDeviceType getType() const;

    /** @brief Gets the name of the graphics card being used. (Ex. Nvidia RTX 2070) */ 
    std::string getDeviceName() const;

    /** @brief Gets the vendor name of the graphics card. (Ex. AMD, Nvidia) */ 
    std::string getVendorName() const;

    /** @brief Gets the index of this physical device as per vkEnumeratePhysicalDevices().  */ 
    uint32_t getIndex() const;

    /** @brief Gets the available present modes on the system. */ 
    std::vector<VkPresentModeKHR> getPresentModes(std::shared_ptr<RenderWindow> window) const;

    /** @brief Gets the available surface formats on this physical device.*/
    std::vector<VkSurfaceFormatKHR> getSurfaceFormats(std::shared_ptr<RenderWindow> window) const;
    
    /** @brief Finds a supported format within a list.
     *
     *  @param candidates List of candidates for a format, order from best to last. 
     *  @param tiling Image tiling option wanted.
     *  @param features Desired format features wanted.
     *  @return A format within the format list within candidates, VK_FORMAT_UNDEFINED if not successful.
     */
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

private:
    VkPhysicalDevice    _physicalDevice;
    uint32_t            _index;
};

}