#pragma once

#include "Window.h"
#include "Instance.h"

namespace bl 
{

class DescriptorSetLayoutCache;
class PipelineLayoutCache;
class DescriptorSetCache;

/** @brief A graphics device used as the basis of many graphics operations. */
class Device 
{
public:
    Device(Instance* instance, PhysicalDevice* physicalDevice); /** @brief Creates the Vulkan graphics device and some other utils. */
    ~Device(); /** @brief Destructor. */

    Instance* GetInstance() const; /** @brief Returns the instance that this device was created with. */
    PhysicalDevice* GetPhysicalDevice() const; /** @brief Returns the physical device this device was crated with. */
    uint32_t GetGraphicsFamilyIndex() const; /** @brief Returns the index used for graphics queue operations. */
    uint32_t GetPresentFamilyIndex() const; /** @brief Returns the index used for present queue operations. */
    bool GetAreQueuesSame() const; /** @brief Returns true if the graphics family index and present family index are the same. */
    VkQueue GetGraphicsQueue() const; /** @brief Returns the Vulkan graphics queue. */
    VkQueue GetPresentQueue() const; /** @brief Returns the Vulkan present queue. */
    VkDevice Get() const; /** @brief Returns the underlying Vulkan device. */
    VkCommandPool GetCommandPool() const; /** @brief Returns the default Vulkan command pool. */
    VmaAllocator GetAllocator() const; /** @brief Returns the Vulkan Memory Allocator object. */
    void ImmediateSubmit(const std::function<void(VkCommandBuffer)>& recorder); /** @brief Submits commands to the graphics queue on the double. */
    void WaitForDevice() const; /** @brief Waits for an undefined amount of time for the device to finish whatever it may be doing. */
    VkDescriptorSetLayout CacheDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    VkPipelineLayout CachePipelineLayout(const std::span<VkDescriptorSetLayout> descriptorSetLayouts, const std::span<VkPushConstantRange> pushConstantsRanges);
    VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);
    void FreeDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet set);
    void PushPostFrameDeleter(std::function<void(void)> func); /** @brief After count frames have been run though, this function will run. */

private:
    std::vector<const char*> GetValidationLayers(); /** @brief Gets the device validation layers required to created the device. */
    std::vector<const char*> GetExtensions(); /** @brief Gets the device's extensions required for the engine. */
    void CreateDevice(); /** @brief Creates the Vulkan device. */
    void CreateCommandPool(); /** @brief Creates a command pool for allocating command buffers. */
    void CreateAllocator(); /** @brief Creates an instance of the Vulkan Memory Allocator */

    Instance* _instance;
    PhysicalDevice* _physicalDevice;
    uint32_t _graphicsFamilyIndex, _presentFamilyIndex;
    VkDevice _device;
    VkQueue _graphicsQueue, _presentQueue;
    VkCommandPool _commandPool;
    VmaAllocator _allocator;
    std::unique_ptr<DescriptorSetLayoutCache> _descriptorSetLayoutCache;
    std::unique_ptr<PipelineLayoutCache> _pipelineLayoutCache;
    std::unique_ptr<DescriptorSetCache> _descriptorSetCache;
};

} // namespace bl
