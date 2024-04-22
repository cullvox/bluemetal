#pragma once

#include "Graphics/Device.h"
#include "Describable.h"

namespace bl 
{

class Buffer : public Describable
{
public:
    Buffer(Device& device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* allocationInfo = nullptr); /** @brief Constructor */
    ~Buffer(); /** @brief Destructor */

    VkBufferUsageFlags GetUsage() const; /** @brief Returns the usage types of the buffer. */
    VkMemoryPropertyFlags GetMemoryProperties() const; /** @brief Returns the memory properties the buffer was created with. */
    VkDeviceSize GetSize() const; /** @brief Returns the size of the buffer in bytes. */
    VmaAllocation GetAllocation() const; /** @brief Returns the underlying VMA allocation. */
    VkBuffer GetBuffer() const; /** @brief Returns the underlying Vulkan buffer. */
    void Upload(VkBufferCopy copyRegion, void* data); /** @brief Uploads a portion of memory to the buffer on the GPU immediately. */
    void Upload(void* data); /** @brief Uploads a portion of memory to the buffer on the GPU immediately. */

private:
    void CreateBuffer(VmaAllocationInfo* allocationInfo);
    void Map(void** mapped);
    void Unmap();

    Device& _device;
    VkBufferUsageFlags _usage;
    VkMemoryPropertyFlags _memoryProperties;
    VkDeviceSize _size;
    VkBuffer _buffer;
    VmaAllocation _allocation;
};

} // namespace bl
