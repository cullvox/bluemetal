#pragma once

#include "VulkanResource.h"
#include <vulkan/vulkan_core.h>

namespace bl {

class VulkanInstance;
class VulkanPhysicalDevice;
class VulkanDescriptorSetLayoutCache;
class VulkanPipelineLayoutCache;
class VulkanDescriptorSetAllocatorCache;
class VulkanDescriptorSet;

/**
 * @class VulkanDevice
 * @brief Represents a Vulkan device used for rendering.
 */
class VulkanDevice : public VulkanResource {
    VulkanInstance* _instance;
    VulkanPhysicalDevice* _physicalDevice;
    uint32_t _graphicsFamilyIndex, _presentFamilyIndex;
    VkDevice _device;
    VkQueue _graphicsQueue, _presentQueue;
    VkCommandPool _commandPool;
    VmaAllocator _allocator;
    std::unique_ptr<VulkanDescriptorSetLayoutCache> _descriptorSetLayoutCache;
    std::unique_ptr<VulkanPipelineLayoutCache> _pipelineLayoutCache;
    std::unique_ptr<VulkanDescriptorSetAllocatorCache> _descriptorSetCache;
    std::unique_ptr<VulkanDescriptorSet> _emptySet;

    std::vector<const char*> GetValidationLayers(); /** @brief Gets the device validation layers required to created the device. */
    std::vector<const char*> GetExtensions(); /** @brief Gets the device's extensions required for the engine. */
    void CreateDevice(); /** @brief Creates the Vulkan device. */
    void CreateCommandPool(); /** @brief Creates a command pool for allocating command buffers. */
    void CreateAllocator(); /** @brief Creates an instance of the Vulkan Memory Allocator */
    void CreateCaches();

public:
    /**
     * @brief Default constructor for VulkanDevice.
     */
    VulkanDevice();

    /**
     * @brief Constructs a VulkanDevice with the specified instance and physical device.
     * @param instance The Vulkan instance to use.
     * @param physicalDevice The physical device to use.
     */
    VulkanDevice(VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice);

    /**
     * @brief Move constructor for VulkanDevice.
     */
    VulkanDevice(VulkanDevice&& other) = default;
    VulkanDevice(const VulkanDevice& copy) = delete;

    /**
     * @brief Destructor for VulkanDevice.
     */
    ~VulkanDevice();

    /**
     * @brief Move assignment operator for VulkanDevice.
     * @param move The VulkanDevice to move from.
     * @return Reference to this VulkanDevice.
     */
    VulkanDevice& operator=(VulkanDevice&& move) noexcept;
    VulkanDevice& operator=(const VulkanDevice& copy) = delete;

    /**
     * @brief Returns the Vulkan instance this device was created with.
     * @return The Vulkan instance.
     */
    VulkanInstance* GetInstance() const;

    /**
     * @brief Returns the physical device this device was created with.
     * @return The Vulkan physical device.
     */
    VulkanPhysicalDevice* GetPhysicalDevice() const;

    /**
     * @brief Returns the graphics family index used for graphics operations.
     * @return The graphics family index.
     */
    uint32_t GetGraphicsFamilyIndex() const;

    /**
     * @brief Returns the present family index used for present operations.
     * @return The present family index.
     */
    uint32_t GetPresentFamilyIndex() const;

    /**
     * @brief Returns true if the graphics family index and present family index are the same.
     * @return True if the indices are the same, false otherwise.
     */
    bool AreQueuesSame() const;

    /**
     * @brief Returns the Vulkan graphics queue.
     * @return The Vulkan graphics queue.
     */
    VkQueue GetGraphicsQueue() const;

    /**
     * @brief Returns the Vulkan present queue.
     * @return The Vulkan present queue.
     */
    VkQueue GetPresentQueue() const;

    /**
     * @brief Returns the underlying Vulkan device.
     * @return The Vulkan device handle.
     */
    VkDevice Get() const;

    /**
     * @brief Returns the Vulkan command pool used for command buffer allocation.
     * @return The Vulkan command pool.
     */
    VkCommandPool GetCommandPool() const;

    /**
     * @brief Returns the Vulkan Memory Allocator used for memory management.
     * @return The Vulkan Memory Allocator.
     */
    VmaAllocator GetAllocator() const;

    /**
     * @brief Immediately submits a command buffer to the graphics queue.
     * @param recorder The function to record commands into the command buffer.
     * This function will create a command buffer, record commands, submit it, and wait for completion.
     */
    void ImmediateSubmit(const std::function<void(VkCommandBuffer)>& recorder);

    /**
     * @brief This function blocks until the device is idle.
     * @details This is useful for ensuring that all operations are complete before proceeding.
     */
    void WaitForDevice();

    /**
     * @brief Acquires a descriptor set layout with the specified bindings.
     * @param bindings The bindings to use for the descriptor set layout.
     * @return The VkDescriptorSetLayout handle.
     */
    VkDescriptorSetLayout AcquireDescriptorSetLayout(std::span<VkDescriptorSetLayoutBinding> bindings);

    /**
     * @brief Acquires a pipeline layout with the specified descriptor set layouts and push constant ranges.
     * @param layouts The descriptor set layouts to use.
     * @param ranges The push constant ranges to use.
     * @return The VkPipelineLayout handle.
     */
    VkPipelineLayout AcquirePipelineLayout(const std::span<VkDescriptorSetLayout> layouts, const std::span<VkPushConstantRange> ranges);


    std::unique_ptr<VulkanDescriptorSet> AllocateDescriptorSet(VkDescriptorSetLayout layout);

    void FreeDescriptorSet(VkDescriptorSet set, VkDescriptorSetLayout layout);

    /**
     * @brief Calculates the dynamic alignment for a uniform buffer.
     * @param uboSize Size of the uniform buffer in bytes.
     * @return The dynamic alignment size for the uniform buffer.
     *
     * This is used to ensure that the uniform buffer is aligned to the
     * dynamic uniform buffer alignment requirements of Vulkan.
     */
    std::size_t GetDynamicAlignment(size_t uboSize);

    /**
     * @brief Gets an empty descriptor set that can be used for binding when a pipeline doesn't use a specific set.
     * @return The VkDescriptorSet handle.
     */
    VkDescriptorSet GetEmptyDescriptorSet() const;
};

} // namespace bl
