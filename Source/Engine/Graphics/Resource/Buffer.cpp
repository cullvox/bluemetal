#include "Buffer.h"
#include "Core/Print.h"

namespace bl {

GfxBuffer::GfxBuffer(
    std::shared_ptr<GfxDevice>  device,
    VkBufferUsageFlags          usage,
    VkMemoryPropertyFlags       memoryProperties,
    VkDeviceSize                size,
    VmaAllocationInfo*          allocationInfo)
    : _device(device)
    , _usage(usage)
    , _memoryProperties(memoryProperties)
    , _size(size)
{
    createBuffer(allocationInfo);
}

GfxBuffer::~GfxBuffer() 
{ 
    vmaDestroyBuffer(_device->getAllocator(), _buffer, _allocation);
}

void GfxBuffer::createBuffer(VmaAllocationInfo* allocationInfo)
{

    // Build the buffer create info.
    uint32_t graphicsFamilyIndex = _device->getGraphicsFamilyIndex();

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = _usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT; // There is never an instance where a buffer isn't a transfer dest, add it just in case the user forgot.
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 1;
    bufferCreateInfo.pQueueFamilyIndices = &graphicsFamilyIndex;

    VmaAllocatorCreateFlags flags = 0;

    // Allocate the buffer.
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = _memoryProperties;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 0.0f;

    VK_CHECK(vmaCreateBuffer(_device->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, allocationInfo))
}

void GfxBuffer::map(void** mapped)
{
    VK_CHECK(vmaMapMemory(_device->getAllocator(), _allocation, mapped))
}

void GfxBuffer::unmap()
{
    vmaUnmapMemory(_device->getAllocator(), _allocation);
}

VmaAllocation GfxBuffer::getAllocation() const { return _allocation; }
VkBuffer GfxBuffer::getBuffer() const { return _buffer; }
VkDeviceSize GfxBuffer::getSize()  const { return _size; }

void GfxBuffer::upload(VkBufferCopy copyRegion, void* srcData)
{
    // Build a host visible intermediate buffer for a quick transfer.
    GfxBuffer intermediateBuffer{
        _device, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
        copyRegion.size};

    void* mapped = nullptr;
    intermediateBuffer.map(&mapped);

    std::memcpy(mapped, srcData, copyRegion.srcOffset + copyRegion.size);

    // The buffer that's created is only the size of the region
    // therefore we don't need the whole data copied only the region.
    // And srcOffset is set to zero to negate this.
    copyRegion.srcOffset = 0; 

    _device->immediateSubmit([&](VkCommandBuffer cmd){ 
        vkCmdCopyBuffer(cmd, intermediateBuffer.getBuffer(), _buffer, 1, &copyRegion); 
    });

    intermediateBuffer.unmap();
}

void GfxBuffer::upload(void* srcData)
{
    VkBufferCopy region{0, 0, getSize()};
    upload(region, srcData);
}


} // namespace bl
