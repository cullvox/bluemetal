#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Core/Language.h"
#include "Graphics/Instance.h"
#include "Window/Window.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

struct GraphicsDeviceCreateInfo
{
    GraphicsInstance*           pInstance;       /// Graphics instance.
    GraphicsPhysicalDevice*     pPhysicalDevice; /// Physical GPU the device is storing resources on.
    Window*                     pWindow;         /// Temporary window only required for creation of the device.
};

/// A graphics device used as the basis of many graphics operations.
class BLUEMETAL_API GraphicsDevice
{
public:

    /// Default Constructor
    GraphicsDevice();

    /// Move Constructor 
    GraphicsDevice(GraphicsDevice&& rhs);

    /// Constructs a graphics device for underlying graphics operations.
    GraphicsDevice(const GraphicsDeviceCreateInfo& createInfo);
    
    /// Default Destructor
    ~GraphicsDevice();

    /// Creates the Vulkan graphics device and some other utils.
    ///
    ///     @param createInfo Other objects/info required to create the device.
    /// 
    /// @return True on success. 
    bool create(const GraphicsDeviceCreateInfo& createInfo);

    /// Deletes the underlying graphics device and it's other objects.
    void destroy() noexcept;

    /// Returns true if the device is created.
    bool isCreated() const noexcept;

public:

    /// Returns the physical device this device was crated with. 
    GraphicsPhysicalDevice* getPhysicalDevice() const;

    /// Returns the underlying Vulkan device. 
    VkDevice getHandle() const;

    /// Returns the index used for graphics queue operations. 
    uint32_t getGraphicsFamilyIndex() const;

    /// Returns the index used for present queue operations. 
    uint32_t getPresentFamilyIndex() const;

    /// Returns the Vulkan graphics queue. 
    VkQueue getGraphicsQueue() const;

    /// Returns the Vulkan present queue.
    VkQueue getPresentQueue() const;

    /// Returns the default Vulkan command pool. 
    VkCommandPool getCommandPool() const;

    /// Returns the Vulkan Memory Allocator object. 
    VmaAllocator getAllocator() const;

    /// Returns true if the graphics family index and present family index are the same. 
    bool areQueuesSame() const;

    /// Submits commands to the graphics queue.
    /// 
    ///     @param recorder Record your commands to a lambda.
    /// 
    /// @return True on successful submission to the graphics queue.
    ///
    bool immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder);
    
    /// Waits for an undefined amount of time for the device to finish whatever it may be doing.
    void waitForDevice();
    
private:

    /// Gets the device validation layers required to created the device.
    ///
    ///     @param[out] layers Layers to add to the device create info.
    ///
    /// @return True if the layers were found.
    ///
    bool getValidationLayers(std::vector<const char*>& layers);

    /// Gets the device's extensions required for the engine.
    ///
    ///     @param[out] extensions Extensions to add to the device create info.
    ///
    /// @return True if the extensions were found.
    /// 
    bool getExtensions(std::vector<const char*>& extensions);
    
    /// Creates the actual Vulkan device.
    /// @return True if the device was created successfully.
    bool createDevice();

    /// Creates a command pool for ease of use later.
    /// @return True if the command pool was created successfully.
    bool createCommandPool();

    /// Creates an instance of the Vulkan Memory Allocator (VMA)
    /// @return True if the allocator was created successfully. 
    bool createAllocator();
    
    GraphicsDeviceCreateInfo    _createInfo;
    GraphicsInstance*           _pInstance;
    GraphicsPhysicalDevice*     _pPhysicalDevice;

    uint32_t                    _graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice                    _device;
    VkQueue                     _graphicsQueue, m_presentQueue;
    VkCommandPool               _commandPool;
    VmaAllocator                _allocator;
};

} // namespace bl
