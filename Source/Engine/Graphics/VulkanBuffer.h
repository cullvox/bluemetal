#pragma once

#include "VulkanDevice.h"

namespace bl {

class VulkanBuffer {
public:
    VulkanBuffer();
    VulkanBuffer(VulkanBuffer&& rhs);
    VulkanBuffer(VulkanDevice* device, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkDeviceSize size, VmaAllocationInfo* allocationInfo = nullptr, bool mapped = false);
    ~VulkanBuffer();

    VulkanBuffer& operator=(VulkanBuffer&& rhs);

    VkBufferUsageFlags GetUsage() const; /** @brief Returns the usage types of the buffer. */
    VkMemoryPropertyFlags GetMemoryProperties() const; /** @brief Returns the memory properties the buffer was created with. */
    VkDeviceSize GetSize() const; /** @brief Returns the size of the buffer in bytes. */
    VmaAllocation GetAllocation() const; /** @brief Returns the underlying VMA allocation. */
    VkBuffer Get() const; /** @brief Returns the underlying Vulkan buffer. */
    void Map(void** mapped);
    void Unmap();
    void Upload(std::span<const std::byte> data); /** @brief Uploads a portion of memory to the buffer on the GPU immediately. */
    void Flush(VkDeviceSize offset, VkDeviceSize size);

private:
    void Cleanup();

    VulkanDevice* _device;
    VkBufferUsageFlags _usage;
    VmaMemoryUsage _memoryUsage;
    VkDeviceSize _size;
    VkBuffer _buffer;
    VmaAllocation _allocation;
};

} // namespace bl
