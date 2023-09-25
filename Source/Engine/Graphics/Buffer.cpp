#include "Buffer.h"
#include "Core/Print.h"

namespace bl {

Buffer::Buffer(const BufferCreateInfo& info)
{
    if (!create(info)) throw std::runtime_error("Could not create a buffer, check prevous logs!");
}

Buffer::~Buffer() 
{ 
    destroy();
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
    destroy();

    _pDevice = rhs._pDevice;
    _size = rhs._size;
    _buffer = rhs._buffer;
    _allocation = rhs._allocation;

    rhs._pDevice = {};
    rhs._size = {};
    rhs._buffer = {};
    rhs._allocation = {};

    return *this;
}

bool Buffer::create(const BufferCreateInfo& info) noexcept
{
    _pDevice = info.pDevice;
    _size = info.size;

    // Build the buffer create info.
    uint32_t graphicsFamilyIndex = _pDevice->getGraphicsFamilyIndex();

    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = _size;
    createInfo.usage = (VkBufferUsageFlags)info.usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &graphicsFamilyIndex;

    // If the user wanted a mapped buffer.
    VmaAllocatorCreateFlags flags = info.mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

    // Allocate the buffer.
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = info.memoryProperties;
    allocationCreateInfo.preferredFlags = 0;
    allocationCreateInfo.memoryTypeBits = 0;
    allocationCreateInfo.pool = VK_NULL_HANDLE;
    allocationCreateInfo.pUserData = nullptr;
    allocationCreateInfo.priority = 0.0f;

    if (vmaCreateBuffer(_pDevice->getAllocator(), &createInfo, &allocationCreateInfo, &_buffer, &_allocation, info.pAllocationInfo) != VK_SUCCESS) {
        blError("Could not allocate a vulkan buffer!");
        return false;
    }

    return true;
}

void Buffer::destroy() noexcept
{
    vmaDestroyBuffer(_pDevice->getAllocator(), _buffer, _allocation);
    _pDevice = nullptr;
    _size = 0;
    _buffer = VK_NULL_HANDLE;
    _allocation = VK_NULL_HANDLE;
}

bool Buffer::isCreated() const noexcept { return _buffer != VK_NULL_HANDLE; }

VmaAllocation Buffer::getAllocation() const noexcept { return _allocation; }
VkBuffer Buffer::getBuffer() const noexcept { return _buffer; }
VkDeviceSize Buffer::getSize()  const noexcept{ return _size; }

bool Buffer::copyFrom(const Buffer& copy)
{
    // TODO: Implement ME!
    return false;
}

} // namespace bl
