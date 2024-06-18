#include "Core/Print.h"
#include "VulkanBuffer.h"

namespace bl {

Buffer::Buffer()
    : _device(nullptr)
    , _usage(0)
    , _memoryProperties(0)
    , _size(0)
    , _buffer(VK_NULL_HANDLE)
    , _allocation(VK_NULL_HANDLE) {}

Buffer::Buffer(Buffer&& rhs)
    : _device(rhs._device)
    , _usage(rhs._usage)
    , _memoryProperties(rhs._memoryProperties)
    , _size(rhs._size)
    , _buffer(rhs._buffer)
    , _allocation(rhs._allocation) {
    rhs._device = nullptr;
    rhs._usage = 0;
    rhs._memoryProperties = 0;
    rhs._size = 0;
    rhs._buffer = VK_NULL_HANDLE;
    rhs._allocation = VK_NULL_HANDLE;
}

Buffer::Buffer(VulkanDevice* device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* allocationInfo)
    : _device(device)
    , _usage(usage)
    , _memoryProperties(memoryProperties)
    , _size(size) {
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

Buffer::~Buffer() {
    Cleanup();
}

Buffer& Buffer::operator=(Buffer&& rhs) {
    if (_buffer != VK_NULL_HANDLE) {
        blWarning("Moving buffer into already created buffer, this isn't expected behavior.");
        Cleanup();
    }

    _device = rhs._device;
    _usage = rhs._usage;
    _memoryProperties = rhs._memoryProperties;
    _size = rhs._size;
    _buffer = rhs._buffer;
    _allocation = rhs._allocation;

    rhs._device = nullptr;
    rhs._usage = 0;
    rhs._memoryProperties = 0;
    rhs._size = 0;
    rhs._buffer = VK_NULL_HANDLE;
    rhs._allocation = VK_NULL_HANDLE;

    return *this;
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

void Buffer::Flush(VkDeviceSize offset, VkDeviceSize size)
{
    VK_CHECK(vmaFlushAllocation(_device->GetAllocator(), _allocation, offset, size))
}

void Buffer::Cleanup()
{
    if (_buffer != VK_NULL_HANDLE)
        vmaDestroyBuffer(_device->GetAllocator(), _buffer, _allocation);
}

} // namespace bl
