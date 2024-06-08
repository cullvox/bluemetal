#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Vulkan.h"

namespace bl {

class Window;

/** @brief A physical GPU within the system. */
class PhysicalDevice : public NonCopyable {
public:
    PhysicalDevice(VkPhysicalDevice device); /** @brief Constructor */
    PhysicalDevice(PhysicalDevice&&) = default;
    ~PhysicalDevice(); /** @brief Destructor */

    VkPhysicalDevice Get() const; /** @brief Gets the underlying VkPhysicalDevice object. */ 
    VkPhysicalDeviceType GetType() const; /** @brief Returns the type of physical devices this is. (Integrated, External... )*/
    std::string GetDeviceName() const; /** @brief Gets the name of the graphics card being used. (Ex. Nvidia RTX 2070) */ 
    std::string GetVendorName() const; /** @brief Gets the vendor name of the graphics card. (Ex. AMD, Nvidia) */ 
    std::vector<VkPresentModeKHR> GetPresentModes(Window* window) const; /** @brief Gets the available present modes on the system. */ 
    std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(Window* window) const; /** @brief Gets the available surface formats on this physical device.*/
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const; /** @brief Finds a supported format given candidates. */
    const VkPhysicalDeviceProperties& GetProperties() const;

private:
    VkPhysicalDevice _physicalDevice;
    VkPhysicalDeviceProperties _properties;
};

}