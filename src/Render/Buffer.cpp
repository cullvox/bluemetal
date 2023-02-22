#include "Buffer.hpp"

CBuffer::CBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, CRenderDevice* const pRenderDevice)
    : m_pRenderDevice(pRenderDevice)
{

    const uint32_t graphicsFamilyIndex = m_pRenderDevice->GetGraphicsFamilyIndex();
    const VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphicsFamilyIndex,
    };

    const VmaAllocationCreateInfo allocationCreateInfo{
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = memoryProperties,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0.0f,
    };

    if (vmaCreateBuffer(m_pRenderDevice->GetAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan buffer!");
    }
}