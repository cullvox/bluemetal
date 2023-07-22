#include "Buffer.h"

namespace bl
{

Buffer::Buffer(std::shared_ptr<Device> device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* pInfo, bool mapped)
    : _device(device)
    , _size(size)
{

    uint32_t graphicsFamilyIndex = _device->getGraphicsFamilyIndex();
    
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = (VkBufferUsageFlags)usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 1;
    bufferCreateInfo.pQueueFamilyIndices = &graphicsFamilyIndex;

    VmaAllocatorCreateFlags flags = mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = memoryProperties;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 0.0f;

    if (vmaCreateBuffer(_device->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, pInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan buffer!");
    }
}

Buffer::~Buffer()
{
    vmaDestroyBuffer(_device->getAllocator(), _buffer, _allocation);
}

VmaAllocation Buffer::getAllocation()
{
    return _allocation;
}

VkBuffer Buffer::getBuffer()
{
    return _buffer;
}

VkDeviceSize Buffer::getSize()
{
    return _size;
}

} // namespace bl