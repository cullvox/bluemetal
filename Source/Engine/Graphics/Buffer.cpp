#include "Buffer.h"
#include "Core/Print.h"

namespace bl {

GfxBuffer::GfxBuffer(std::shared_ptr<GfxDevice> device, const CreateInfo& createInfo)
    : _device(device)
{
    createBuffer(createInfo);
}

GfxBuffer::~GfxBuffer() 
{ 
    vmaDestroyBuffer(_device->getAllocator(), _buffer, _allocation);
}

void GfxBuffer::createBuffer(const CreateInfo& createInfo)
{
    _size = createInfo.size;

    // Build the buffer create info.
    uint32_t graphicsFamilyIndex = _device->getGraphicsFamilyIndex();

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = (VkBufferUsageFlags)createInfo.usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 1;
    bufferCreateInfo.pQueueFamilyIndices = &graphicsFamilyIndex;

    // If the user wanted a mapped buffer.
    VmaAllocatorCreateFlags flags = createInfo.mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

    // Allocate the buffer.
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = createInfo.memoryProperties;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 0.0f;

    if (vmaCreateBuffer(_device->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, createInfo.pAllocationInfo) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate a vulkan buffer!");
    }
}

VmaAllocation GfxBuffer::getAllocation() const { return _allocation; }
VkBuffer GfxBuffer::getBuffer() const { return _buffer; }
VkDeviceSize GfxBuffer::getSize()  const { return _size; }

} // namespace bl
