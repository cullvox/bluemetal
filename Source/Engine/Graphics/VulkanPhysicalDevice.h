#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Vulkan.h"

namespace bl {

class VulkanWindow;

/// @brief A physical GPU within the system.
class VulkanPhysicalDevice {
public:
    /// @brief Default Constructor
    VulkanPhysicalDevice() = default;

    /// @brief Physical Device Constructor
    VulkanPhysicalDevice(VkPhysicalDevice device);
    
    /// @brief Copy Constructor
    /// @param[inout] device The source device object to copy from.
    VulkanPhysicalDevice(VulkanPhysicalDevice& device) = default;

    /// @brief Move Constructor
    /// @param[inout] device The source device object to move from.
    VulkanPhysicalDevice(VulkanPhysicalDevice&& device) = default;
    
    /// @brief Destructor
    ~VulkanPhysicalDevice() = default; /** @brief Destructor */

public:
    /// @brief Get
    /// @returns The underlying VkPhysicalDevice object.
    VkPhysicalDevice Get() const; 

    /// @brief GetType
    /// @returns The type of physical devices this is. (Integrated, External... )
    VkPhysicalDeviceType GetType() const;
    
    /// @brief GetDeviceName
    /// @returns A user readable name of this physical device. (Ex. Nvidia RTX 2070)
    std::string GetDeviceName() const;

    /// @brief GetVendorName
    /// @returns A user readable name of who created this physical device. (Ex. AMD, Nvidia)
    std::string GetVendorName() const;

    /// @brief GetPresentModes
    /// @returns Available present modes when using this device.
    const std::vector<VkPresentModeKHR>& GetPresentModes(VulkanWindow* window);

    /// @brief GetSurfaceFormats
    /// @returns Available surface formats when using this device.
    const std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats(VulkanWindow* window);

    std::optional<VkImageFormatProperties2> GetImageFormatProperties(const VkPhysicalDeviceImageFormatInfo2& info);

    /// @brief FindSupportedFormat
    /// @param candidates[in] List of possible formats to choose from.
    /// @param tiling[in] Selected required tiling option for the format.
    /// @param features[in] Select a format which supports a specific set of features. 
    /// @returns Finds a supported format given candidates.
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
    
    /// @brief GetProperties
    /// @returns The Vulkan physical device properties.
    const VkPhysicalDeviceProperties& GetProperties() const;

private:
    VkPhysicalDevice _physicalDevice;
    VkPhysicalDeviceProperties _properties;
    std::vector<VkPresentModeKHR> _presentModes;
    std::vector<VkSurfaceFormatKHR> _surfaceFormats;
};

} // namespace bl