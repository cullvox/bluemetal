#pragma once

#include "Device.h"

namespace bl {

class BLUEMETAL_API GfxBuffer
{
public:
    /** @brief Constructor */
    GfxBuffer(
        std::shared_ptr<GfxDevice>  device, 
        VkBufferUsageFlags          usage,                      /** @brief Vulkan memory usage, specifing buffer type. */
        VkMemoryPropertyFlags       memoryProperties,           /** @brief Vulkan memory properies, how this buffer will be used. */
        VkDeviceSize                size,                       /** @brief Size of your buffer in bytes. */
        VmaAllocationInfo*          allocationInfo = nullptr);  /** @brief Output value, additional allocation info if needed. */

    /** @brief Destructor */
    ~GfxBuffer();

public:

    /** @brief Returns the usage types of the buffer. */
    VkBufferUsageFlags getUsage() const;

    /** @brief Returns the memory properties the buffer was created with. */
    VkMemoryPropertyFlags getMemoryProperties() const;

    /** @brief Returns the size of the buffer in bytes. */
    VkDeviceSize getSize() const;

    /** @brief Returns the underlying VMA allocation. */
    VmaAllocation getAllocation() const;

    /** @brief Returns the underlying Vulkan buffer. */
    VkBuffer getBuffer() const;

    /** @brief Uploads a portion of memory to the buffer on the GPU immediately. */
    void upload(VkBufferCopy copyRegion, void* data);

private:
    void createBuffer(VmaAllocationInfo* allocationInfo);
    void map(void** mapped);
    void unmap();

    std::shared_ptr<GfxDevice>  _device;
    VkBufferUsageFlags          _usage;
    VkMemoryPropertyFlags       _memoryProperties;
    VkDeviceSize                _size;
    VkBuffer                    _buffer;
    VmaAllocation               _allocation;
};

} // namespace bl
