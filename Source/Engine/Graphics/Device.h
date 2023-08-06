#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Graphics/Instance.h"
#include "Window/Window.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

/// A graphics device used as the basis of many graphics operations.
class BLUEMETAL_API GraphicsDevice
{
public:

    /// Constructs a graphics device for underlying graphics operations.
    /// 
    ///     @param instance Graphics instance.
    ///     @param physicalDevice Physical GPU the device is storing resources on.
    ///     @param tempWindow Temporary window only required for creation of the device.
    ///
    GraphicsDevice(
        GraphicsInstance*               pInstance, 
        GraphicsPhysicalDevice*         pPhysicalDevice, 
        Window*                         pTempWindow);
    
    /// Move Constructor 
    GraphicsDevice(GraphicsDevice&&     other);
    
    /// Default Destructor
    ~GraphicsDevice();

    /// Returns the physical device this device was crated with. 
    GraphicsPhysicalDevice* getPhysicalDevice();

    /// Returns the underlying Vulkan device. 
    VkDevice getHandle();

    /// Returns the index used for graphics queue operations. 
    uint32_t getGraphicsFamilyIndex();

    /// Returns the index used for present queue operations. 
    uint32_t getPresentFamilyIndex();

    /// Returns the Vulkan graphics queue. 
    VkQueue getGraphicsQueue();

    /// Returns the Vulkan present queue.
    VkQueue getPresentQueue();

    /// Returns the default Vulkan command pool. 
    VkCommandPool getCommandPool();

    /// Returns the Vulkan Memory Allocator object. 
    VmaAllocator getAllocator();

    /// Returns true if the graphics family index and present family index are the same. 
    bool areQueuesSame();
    
    VkFormat findSupportedFormat(
        const std::vector<VkFormat>&    candidates, 
        VkImageTiling                   tiling, 
        VkFormatFeatureFlags            features);

    void immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder);
    
    void waitForDevice();
    
private:
    std::vector<const char*> getValidationLayers();
    std::vector<const char*> getExtensions();
    void createDevice(Window* window);
    void createCommandPool();
    void createAllocator();

    GraphicsInstance*       m_instance;
    GraphicsPhysicalDevice* m_physicalDevice;
    uint32_t                m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice                m_device;
    VkQueue                 m_graphicsQueue, m_presentQueue;
    VkCommandPool           m_commandPool;
    VmaAllocator            m_allocator;
};

using BLGraphicsDevice = GraphicsDevice*;

} // namespace bl