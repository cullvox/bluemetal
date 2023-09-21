#pragma once

#include "Graphics/Instance.h"
#include "Window/Window.h"

namespace bl {

struct GraphicsDeviceCreateInfo
{
    GraphicsInstance*           pInstance;       /** @brief Graphics instance. */
    GraphicsPhysicalDevice*     pPhysicalDevice; /** @brief Physical GPU the device is storing resources on. */
    Window*                     pWindow;         /** @brief Temporary window only required for creation of the device. */
};

/** @brief A graphics device used as the basis of many graphics operations. */
class BLUEMETAL_API GraphicsDevice {
public:

    /** @brief Default Constructor */
    GraphicsDevice();

    /** @brief Move Constructor */ 
    GraphicsDevice(GraphicsDevice&& rhs);

    /** @brief Create Constructor */
    GraphicsDevice(const GraphicsDeviceCreateInfo& info);
    
    /** @brief Default Destructor */
    ~GraphicsDevice();

    /** @brief Creates the Vulkan graphics device and some other utils. */
    [[nodiscard]] bool create(const GraphicsDeviceCreateInfo& createInfo);

    /** @brief Deletes the underlying graphics device and it's other objects. */
    void destroy() noexcept;

    /** @brief Returns true if the device is created. */
    [[nodiscard]] bool isCreated() const noexcept;

public:

    /** @brief Returns the physical device this device was crated with. */
    [[nodiscard]] GraphicsPhysicalDevice* getPhysicalDevice() const noexcept;

    /** @brief Returns the underlying Vulkan device. */
    [[nodiscard]] VkDevice getHandle() const noexcept;

    /** @brief Returns the index used for graphics queue operations. */
    [[nodiscard]] uint32_t getGraphicsFamilyIndex() const noexcept;

    /** @brief Returns the index used for present queue operations. */
    [[nodiscard]] uint32_t getPresentFamilyIndex() const noexcept;

    /** @brief Returns the Vulkan graphics queue. */
    [[nodiscard]] VkQueue getGraphicsQueue() const noexcept;

    /** @brief Returns the Vulkan present queue. */
    [[nodiscard]] VkQueue getPresentQueue() const noexcept;

    /** @brief Returns the default Vulkan command pool. */
    [[nodiscard]] VkCommandPool getCommandPool() const noexcept;

    /** @brief Returns the Vulkan Memory Allocator object. */
    [[nodiscard]] VmaAllocator getAllocator() const noexcept;

    /** @brief Returns true if the graphics family index and present family index are the same. *//
    [[nodiscard]] bool areQueuesSame() const noexcept;

    /** @brief Submits commands to the graphics queue. */
    [[nodicard]] bool immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder);
    
    /** @brief Waits for an undefined amount of time for the device to finish whatever it may be doing. */
    void waitForDevice() const noexcept;
    
private:

    /** @brief Gets the device validation layers required to created the device. */
    bool getValidationLayers(std::vector<const char*>& layers);

    /** @brief Gets the device's extensions required for the engine. */
    bool getExtensions(std::vector<const char*>& extensions);
    
    /** @brief Creates the Vulkan device. */
    bool createDevice();

    /** @brief Creates a command pool for allocating command buffers. */
    bool createCommandPool();

    /** @brief Creates an instance of the Vulkan Memory Allocator */
    bool createAllocator();
    
    GraphicsDeviceCreateInfo    _info;
    GraphicsInstance*           _pInstance;
    GraphicsPhysicalDevice*     _pPhysicalDevice;
    uint32_t                    _graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice                    _device;
    VkQueue                     _graphicsQueue, m_presentQueue;
    VkCommandPool               _commandPool;
    VmaAllocator                _allocator;
};

} // namespace bl
