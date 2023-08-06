#pragma once

#include "Device.h"

namespace bl
{

class BLUEMETAL_API Buffer
{
public:

    /// Creates a graphics buffer on the host or physical device.
    /// 
    ///     @param device Device to use to create the buffer with.
    ///     @param usage Vulkan usages that this buffer should work with.
    ///     @param memoryProperties Vulkan memory properties point to where to create the buffer.
    ///     @param size The size in bytes, how big of a buffer to create.
    ///     @param pAllocationInfo Special allocation info given by Vma.
    ///     @param mapped If we want to map the memory immediately to allow write. (use pAllocationInfo to get) 
    ///
    Buffer(
        GraphicsDevice*         pDevice, 
        VkBufferUsageFlags      usage, 
        VkMemoryPropertyFlags   memoryProperties, 
        VkDeviceSize            size, 
        VmaAllocationInfo*      pAllocationInfo = nullptr, 
        bool                    mapped = false);

    /// Move Constructor
    Buffer(Buffer&& move);

    /// Default Destructor
    ~Buffer();

    VkDeviceSize getSize();
    VkBuffer getBuffer();
    VmaAllocation getAllocation();

private:
    GraphicsDevice* m_pDevice;
    VkDeviceSize    m_size;
    VkBuffer        m_buffer;
    VmaAllocation   m_allocation;
};

} // namespace bl