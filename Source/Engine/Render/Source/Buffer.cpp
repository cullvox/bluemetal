#include "Core/Macros.hpp"
#include "Render/Buffer.hpp"

blBuffer::blBuffer(const std::shared_ptr<blRenderDevice> renderDevice, 
        VkDeviceSize size, VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags memoryProperties)
    : _renderDevice(renderDevice)
    , _size(size)
{

    const uint32_t graphicsFamilyIndex = _renderDevice->getGraphicsFamilyIndex();
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

    if (vmaCreateBuffer(_renderDevice->getAllocator(), &bufferCreateInfo, 
            &allocationCreateInfo, &_buffer, &_allocation, nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan buffer!");
    }
}

blBuffer& blBuffer::operator=(blBuffer&& rhs) noexcept
{
    collapse();

    _renderDevice = std::move(rhs._renderDevice);
    _size = std::move(rhs._size);
    _buffer = std::move(rhs._buffer);
    _allocation = std::move(rhs._allocation);

    rhs.collapse();
    return *this;
}

VmaAllocation blBuffer::getAllocation() const noexcept
{
    return _allocation;
}

VkBuffer blBuffer::getBuffer() const noexcept
{
    return _buffer;
}

VkDeviceSize blBuffer::getSize() const noexcept
{
    return _size;
}

void blBuffer::upload(size_t size, const uint8_t* pData)
{
}

void blBuffer::collapse() noexcept
{
    vmaDestroyBuffer(_renderDevice->getAllocator(), _buffer, _allocation);

    _renderDevice = nullptr;
    _buffer = VK_NULL_HANDLE;
    _allocation = VK_NULL_HANDLE;
    _size = 0;
}