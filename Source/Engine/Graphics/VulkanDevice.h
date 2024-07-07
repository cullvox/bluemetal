#pragma once

#include "Vulkan.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

namespace bl {

/// @brief A graphics device used as the basis of many graphics operations.
class VulkanDevice : public NonCopyable {
public:
    
    /// @brief Default Constructor
    VulkanDevice();

    /// @brief Device Constructor
    /// @param[in] instance The vulkan instance to build this device from.
    /// @param[in] physicalDevice The physical device this device will use.
    /// @param[in] temporarySurface Used only to find a present queue used in the device, can be destroyed after construction.
    VulkanDevice(
        const VulkanInstance* instance, 
        const VulkanPhysicalDevice* physicalDevice, 
        VkSurfaceKHR temporarySurface);
    
    /// @brief Move Constructor
    VulkanDevice(VulkanDevice&& other) = default;

    /// @brief Destructor
    ~VulkanDevice(); /** @brief Destructor. */

    const VulkanInstance* GetInstance() const; /** @brief Returns the instance that this device was created with. */
    const VulkanPhysicalDevice* GetPhysicalDevice() const; /** @brief Returns the physical device this device was crated with. */
    uint32_t GetGraphicsFamilyIndex() const; /** @brief Returns the index used for graphics queue operations. */
    uint32_t GetPresentFamilyIndex() const; /** @brief Returns the index used for present queue operations. */
    bool GetAreQueuesSame() const; /** @brief Returns true if the graphics family index and present family index are the same. */
    VkQueue GetGraphicsQueue() const; /** @brief Returns the Vulkan graphics queue. */
    VkQueue GetPresentQueue() const; /** @brief Returns the Vulkan present queue. */
    VkDevice Get() const; /** @brief Returns the underlying Vulkan device. */
    VkCommandPool GetCommandPool() const; /** @brief Returns the default Vulkan command pool. */
    VmaAllocator GetAllocator() const; /** @brief Returns the Vulkan Memory Allocator object. */
    void ImmediateSubmit(const std::function<void(VkCommandBuffer)>& recorder) const; /** @brief Submits commands to the graphics queue on the double. */

    /// @brief Waits for an undefined amount of time for the device to finish whatever it may be doing.
    void WaitForDevice() const;

private:
    std::vector<const char*> GetValidationLayers(); /** @brief Gets the device validation layers required to created the device. */
    std::vector<const char*> GetExtensions(); /** @brief Gets the device's extensions required for the engine. */
    void CreateDevice(VkSurfaceKHR temporarySurface); /** @brief Creates the Vulkan device. */
    void CreateCommandPool(); /** @brief Creates a command pool for allocating command buffers. */
    void CreateAllocator(); /** @brief Creates an instance of the Vulkan Memory Allocator */

    const VulkanInstance* _instance;
    const VulkanPhysicalDevice* _physicalDevice;
    uint32_t _graphicsFamilyIndex, _presentFamilyIndex;
    VkDevice _device;
    VkQueue _graphicsQueue, _presentQueue;
    VkCommandPool _commandPool;
    VmaAllocator _allocator;
};

} // namespace bl
