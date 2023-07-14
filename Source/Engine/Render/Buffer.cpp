#include "Buffer.h"

blBuffer::blBuffer(std::shared_ptr<blDevice> device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* pInfo, bool mapped)
    : _device(device)
    , _size(size)
{

    uint32_t graphicsFamilyIndex = _device->getGraphicsFamilyIndex();
    
    const VkBufferCreateInfo bufferCreateInfo
    {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // sType
        nullptr,                                // pNext
        0,                                      // flags
        size,                                   // size
        (VkBufferUsageFlags)usage,              // usage
        VK_SHARING_MODE_EXCLUSIVE,              // sharingMode
        1,                                      // queueFamilyIndexCount
        &graphicsFamilyIndex,                   // pQueueFamilyIndices
    };

    VmaAllocatorCreateFlags flags = mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

    const VmaAllocationCreateInfo allocationCreateInfo
    {
        flags,                      // flags
        VMA_MEMORY_USAGE_GPU_ONLY,  // usage
        memoryProperties,           // requiredFlags
        0,                          // preferredFlags
        0,                          // memoryTypeBits
        VK_NULL_HANDLE,             // pool
        nullptr,                    // pUserData
        0.0f,                       // priority
    };

    if (vmaCreateBuffer(_device->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, pInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan buffer!");
    }
}

blBuffer::~blBuffer()
{
    vmaDestroyBuffer(_device->getAllocator(), _buffer, _allocation);
}

VmaAllocation blBuffer::getAllocation()
{
    return _allocation;
}

VkBuffer blBuffer::getBuffer()
{
    return _buffer;
}

VkDeviceSize blBuffer::getSize()
{
    return _size;
}