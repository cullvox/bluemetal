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

    /// Returns true if the device was created.
    ///
    ///     @param[out] err Human readable expression of what went wrong.
    ///
    bool isCreated(std::string& err) const noexcept;

    /// Returns a human readable string describing the latest error.
    const std::string& getError() const noexcept;

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

    std::string                 m_err;
    GraphicsDeviceCreateInfo    m_createInfo;
    GraphicsInstance*           m_pInstance;
    GraphicsPhysicalDevice*     m_pPhysicalDevice;

    uint32_t                    m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice                    m_device;
    VkQueue                     m_graphicsQueue, m_presentQueue;
    VkCommandPool               m_commandPool;
    VmaAllocator                m_allocator;
};

} // namespace bl