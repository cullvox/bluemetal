#include "Buffer.h"
#include "Core/Print.h"

namespace bl 
{

Buffer::Buffer(Device* device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* allocationInfo)
    : _device(device)
    , _usage(usage)
    , _memoryProperties(memoryProperties)
    , _size(size)
{
    CreateBuffer(allocationInfo);
}

Buffer::~Buffer() 
{ 
    vmaDestroyBuffer(_device->GetAllocator(), _buffer, _allocation);
}

void Buffer::CreateBuffer(VmaAllocationInfo* allocationInfo)
{
    // Build the buffer create info.
    uint32_t graphicsFamilyIndex = _device->GetGraphicsFamilyIndex();

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

    VK_CHECK(vmaCreateBuffer(_device->GetAllocator(), &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, allocationInfo))
}

VmaAllocation Buffer::GetAllocation() const 
{ 
    return _allocation; 
}

VkBuffer Buffer::Get() const 
{ 
    return _buffer; 
}

VkDeviceSize Buffer::GetSize() const 
{ 
    return _size; 
}

void Buffer::Map(void** mapped)
{
    VK_CHECK(vmaMapMemory(_device->GetAllocator(), _allocation, mapped))
}

void Buffer::Unmap()
{
    vmaUnmapMemory(_device->GetAllocator(), _allocation);
}

void Buffer::Upload(VkBufferCopy copyRegion, void* srcData)
{
    // Build a host visible intermediate buffer for a quick transfer.
    Buffer intermediateBuffer{
        _device, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
        copyRegion.size};

    void* mapped = nullptr;
    intermediateBuffer.Map(&mapped);

    std::memcpy(mapped, srcData, copyRegion.srcOffset + copyRegion.size);

    // The buffer that's created is only the size of the region
    // therefore we don't need the whole data copied only the region.
    // And srcOffset is set to zero to negate this.
    copyRegion.srcOffset = 0; 

    _device->ImmediateSubmit([&](VkCommandBuffer cmd){ 
        vkCmdCopyBuffer(cmd, intermediateBuffer.Get(), _buffer, 1, &copyRegion); 
    });

    intermediateBuffer.Unmap();
}

void Buffer::Upload(void* srcData)
{
    VkBufferCopy region{0, 0, GetSize()};
    Upload(region, srcData);
}


} // namespace bl
