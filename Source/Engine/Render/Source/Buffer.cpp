#include "Core/Macros.hpp"
#include "Render/Buffer.hpp"
#include <vulkan/vulkan_core.h>

blBuffer::blBuffer(
        std::shared_ptr<blRenderDevice> renderDevice, 
        vk::BufferUsageFlags usage, 
        vk::MemoryPropertyFlags memoryProperties,
        vk::DeviceSize size,
        VmaAllocationInfo* BL_NULLABLE pInfo,
        bool mapped)
    : _renderDevice(renderDevice)
    , _size(size)
{

    const uint32_t graphicsFamilyIndex = _renderDevice->getGraphicsFamilyIndex();
    const VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = size,
        .usage = (VkBufferUsageFlags)usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &graphicsFamilyIndex,
    };

    const VmaAllocationCreateInfo allocationCreateInfo{
        .flags = mapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : (VmaAllocationCreateFlags)0,
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = (VkMemoryPropertyFlags)memoryProperties,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0.0f,
    };

    VkBuffer tempBuffer;
    if (vmaCreateBuffer(_renderDevice->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &tempBuffer, &_allocation, pInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan buffer!");
    }

    _buffer = tempBuffer;
}

blBuffer::blBuffer(
    std::shared_ptr<blRenderDevice> renderDevice, 
    vk::BufferUsageFlags usage,
    vk::DeviceSize size,
    const void* pData)

    // call previous constructor for a gpu buffer 
    :   blBuffer(renderDevice, usage | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal, size)

{

    // create a temporary cpu sided staging buffer
    VmaAllocationInfo info{};
    blBuffer stagingBuffer(_renderDevice, usage | vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible, size, &info, true);

    // copy to the mapped data pointer
    std::memcpy(info.pMappedData, pData, size);

    // copy the cpu staging buffer to the gpu buffer
    _renderDevice->immediateSubmit([&](vk::CommandBuffer cmd){
        const std::array regions = { vk::BufferCopy(0, 0, size) };
        cmd.copyBuffer(stagingBuffer.getBuffer(), _buffer, regions);
    });

    // success
}

blBuffer::~blBuffer() noexcept
{
    vmaDestroyBuffer(_renderDevice->getAllocator(), _buffer, _allocation);
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

vk::Buffer blBuffer::getBuffer() const noexcept
{
    return _buffer;
}

vk::DeviceSize blBuffer::getSize() const noexcept
{
    return _size;
}

void blBuffer::collapse() noexcept
{
    vmaDestroyBuffer(_renderDevice->getAllocator(), _buffer, _allocation);

    _renderDevice = nullptr;
    _buffer = VK_NULL_HANDLE;
    _allocation = VK_NULL_HANDLE;
    _size = 0;
}