#include "Buffer.h"

namespace bl
{

Buffer::Buffer(
    GraphicsDevice*         pDevice, 
    VkBufferUsageFlags      usage, 
    VkMemoryPropertyFlags   memoryProperties, 
    VkDeviceSize            size, 
    VmaAllocationInfo*      pInfo, 
    bool                    mapped)
    : m_pDevice(pDevice)
    , m_size(size)
{

    // Build the buffer create info.
    uint32_t graphicsFamilyIndex = m_pDevice->getGraphicsFamilyIndex();
    
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = (VkBufferUsageFlags)usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 1;
    bufferCreateInfo.pQueueFamilyIndices = &graphicsFamilyIndex;

    // If the user wanted a mapped buffer.
    VmaAllocatorCreateFlags flags = mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

    // Allocate the buffer.
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = memoryProperties;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 0.0f;

    if (vmaCreateBuffer(m_pDevice->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_buffer, &m_allocation, pInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan buffer!");
    }
}

Buffer::~Buffer()
{
    vmaDestroyBuffer(m_pDevice->getAllocator(), m_buffer, m_allocation);
}

VmaAllocation Buffer::getAllocation()
{
    return m_allocation;
}

VkBuffer Buffer::getBuffer()
{
    return m_buffer;
}

VkDeviceSize Buffer::getSize()
{
    return m_size;
}

} // namespace bl