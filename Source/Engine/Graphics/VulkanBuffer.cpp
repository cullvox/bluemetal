#include "VulkanBuffer.h"
#include "Core/Print.h"

namespace bl {

VulkanBuffer::VulkanBuffer()
    : _device(nullptr)
    , _usage(0)
    , _memoryUsage(VMA_MEMORY_USAGE_UNKNOWN)
    , _size(0)
    , _buffer(VK_NULL_HANDLE)
    , _allocation(VK_NULL_HANDLE)
{
}

VulkanBuffer::VulkanBuffer(VulkanBuffer&& rhs) noexcept
    : _device(rhs._device)
    , _usage(rhs._usage)
    , _memoryUsage(rhs._memoryUsage)
    , _size(rhs._size)
    , _buffer(rhs._buffer)
    , _allocation(rhs._allocation)
{
    // rhs._device = nullptr;
    rhs._usage = 0;
    rhs._memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;
    rhs._size = 0;
    rhs._buffer = VK_NULL_HANDLE;
    rhs._allocation = VK_NULL_HANDLE;
}

static std::string VkBufferUsageFlags_ToString(VkBufferUsageFlags usage)
{
    std::string out;
    if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        out += "VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ";
    if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        out += "VK_BUFFER_USAGE_INDEX_BUFFER_BIT";

    return out;
}

VulkanBuffer::VulkanBuffer(VulkanDevice* device, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkDeviceSize size, VmaAllocationInfo* allocationInfo, bool mapped, VmaAllocationCreateFlags flags)
    : _device(device)
    , _usage(usage)
    , _memoryUsage(memoryUsage)
    , _size(size)
{
    uint32_t graphicsFamilyIndex = _device->GetGraphicsFamilyIndex();

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = _size;
    bufferCreateInfo.usage = _usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 1;
    bufferCreateInfo.pQueueFamilyIndices = &graphicsFamilyIndex;

    flags |= mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.requiredFlags = 0;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 0.0f;

    VK_CHECK(vmaCreateBuffer(_device->GetAllocator(), &bufferCreateInfo, &allocationCreateInfo, &_buffer, &_allocation, allocationInfo))

    vmaSetAllocationName(_device->GetAllocator(), _allocation, VkBufferUsageFlags_ToString(usage).c_str());
}

VulkanBuffer::~VulkanBuffer()
{
    Cleanup();
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& rhs) noexcept
{
    if (_buffer != VK_NULL_HANDLE) {
        Print::Warn("Moving buffer into already created buffer, this isn't expected behavior.");
        Cleanup();
    }

    _device = rhs._device;
    _usage = rhs._usage;
    _memoryUsage = rhs._memoryUsage;
    _size = rhs._size;
    _buffer = rhs._buffer;
    _allocation = rhs._allocation;

    rhs._device = nullptr;
    rhs._usage = 0;
    rhs._memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;
    rhs._size = 0;
    rhs._buffer = VK_NULL_HANDLE;
    rhs._allocation = VK_NULL_HANDLE;

    return *this;
}

VmaAllocation VulkanBuffer::GetAllocation() const
{
    return _allocation;
}

VkBuffer VulkanBuffer::Get() const
{
    return _buffer;
}

VkDeviceSize VulkanBuffer::GetSize() const
{
    return _size;
}

void VulkanBuffer::Map(void** mapped)
{
    VK_CHECK(vmaMapMemory(_device->GetAllocator(), _allocation, mapped))
}

void VulkanBuffer::Unmap()
{
    vmaUnmapMemory(_device->GetAllocator(), _allocation);
}

void VulkanBuffer::Upload(std::span<const std::byte> data)
{
    if (data.size() != _size) {
        throw std::range_error("Buffer upload size mismatch.");
    }

    Upload(0, data);
}

void VulkanBuffer::Upload(size_t offset, std::span<const std::byte> data)
{
    if (offset + data.size() > _size) {
        Print::Error("Buffer upload range exceeds buffer size.");
        return;
    }

    // Build a host visible intermediate buffer for a quick transfer.
    VmaAllocationInfo allocInfo = {};
    VulkanBuffer stagingBuffer { _device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, data.size(), &allocInfo, true };

    std::memcpy(allocInfo.pMappedData, data.data(), data.size());

    _device->ImmediateSubmit([&](VkCommandBuffer cmd) {
        VkBufferCopy region = {};
        region.srcOffset = 0;
        region.dstOffset = offset;
        region.size = data.size();

        vkCmdCopyBuffer(cmd, stagingBuffer.Get(), _buffer, 1, &region);
    });
}

void VulkanBuffer::Flush(VkDeviceSize offset, VkDeviceSize size)
{
    VK_CHECK(vmaFlushAllocation(_device->GetAllocator(), _allocation, offset, size))
}

void VulkanBuffer::Cleanup()
{
    if (_buffer != VK_NULL_HANDLE)
        vmaDestroyBuffer(_device->GetAllocator(), _buffer, _allocation);
}

} // namespace bl
