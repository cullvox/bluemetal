#pragma once

#include "VulkanDevice.h"

namespace bl {

class Buffer {
public:
    Buffer();
    Buffer(Buffer&& rhs);
    Buffer(VulkanDevice* device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* allocationInfo = nullptr);
    ~Buffer();

    Buffer& operator=(Buffer&& rhs);

    VkBufferUsageFlags GetUsage() const; /** @brief Returns the usage types of the buffer. */
    VkMemoryPropertyFlags GetMemoryProperties() const; /** @brief Returns the memory properties the buffer was created with. */
    VkDeviceSize GetSize() const; /** @brief Returns the size of the buffer in bytes. */
    VmaAllocation GetAllocation() const; /** @brief Returns the underlying VMA allocation. */
    VkBuffer Get() const; /** @brief Returns the underlying Vulkan buffer. */
    void Map(void** mapped);
    void Unmap();
    void Upload(VkBufferCopy copyRegion, void* data); /** @brief Uploads a portion of memory to the buffer on the GPU immediately. */
    void Upload(void* data); /** @brief Uploads a portion of memory to the buffer on the GPU immediately. */
    void Flush(VkDeviceSize offset, VkDeviceSize size);

private:
    void Cleanup();

    VulkanDevice* _device;
    VkBufferUsageFlags _usage;
    VkMemoryPropertyFlags _memoryProperties;
    VkDeviceSize _size;
    VkBuffer _buffer;
    VmaAllocation _allocation;
};

} // namespace bl
