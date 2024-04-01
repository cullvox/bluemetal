#pragma once

#include "Graphics/Instance.h"
#include "Window/Window.h"

namespace bl {

/** @brief A graphics device used as the basis of many graphics operations. */
class BLUEMETAL_API GfxDevice {
public:

    struct CreateInfo
    {
        std::shared_ptr<GfxInstance>           instance;       /** @brief Graphics instance. */
        std::shared_ptr<GfxPhysicalDevice>     physicalDevice; /** @brief Physical GPU the device is storing resources on. */
        std::shared_ptr<Window>                window;         /** @brief Temporary window only required for creation of the device. */
    };

    /** @brief Creates the Vulkan graphics device and some other utils. */
    GfxDevice(const CreateInfo& createInfo);
    
    /** @brief Destroys the graphics device. */
    ~GfxDevice();

public:

    /** @brief Returns the underlying Vulkan device. */
    VkDevice get() const;

    /** @brief Returns the physical device this device was crated with. */
    std::shared_ptr<GfxPhysicalDevice> getPhysicalDevice() const;

    /** @brief Returns the index used for graphics queue operations. */
    uint32_t getGraphicsFamilyIndex() const;

    /** @brief Returns the index used for present queue operations. */
    uint32_t getPresentFamilyIndex() const;

    /** @brief Returns the Vulkan graphics queue. */
    VkQueue getGraphicsQueue() const;

    /** @brief Returns the Vulkan present queue. */
    VkQueue getPresentQueue() const;

    /** @brief Returns the default Vulkan command pool. */
    VkCommandPool getCommandPool() const;

    /** @brief Returns the Vulkan Memory Allocator object. */
    VmaAllocator getAllocator() const;

    /** @brief Returns true if the graphics family index and present family index are the same. */
    bool areQueuesSame() const;

    /** @brief Submits commands to the graphics queue. */
    bool immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder);
    
    /** @brief Waits for an undefined amount of time for the device to finish whatever it may be doing. */
    void waitForDevice() const;
    
private:

    /** @brief Gets the device validation layers required to created the device. */
    std::vector<const char*> getValidationLayers();

    /** @brief Gets the device's extensions required for the engine. */
    std::vector<const char*> getExtensions();
    
    /** @brief Creates the Vulkan device. */
    void createDevice(std::shared_ptr<Window> window);

    /** @brief Creates a command pool for allocating command buffers. */
    void createCommandPool();

    /** @brief Creates an instance of the Vulkan Memory Allocator */
    void createAllocator();
    
    std::shared_ptr<GfxInstance>        _instance;
    std::shared_ptr<GfxPhysicalDevice>  _physicalDevice;
    uint32_t                            _graphicsFamilyIndex, _presentFamilyIndex;
    VkDevice                            _device;
    VkQueue                             _graphicsQueue, _presentQueue;
    VkCommandPool                       _commandPool;
    VmaAllocator                        _allocator;
};

} // namespace bl
