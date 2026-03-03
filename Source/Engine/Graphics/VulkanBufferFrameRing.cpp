#include "VulkanBufferFrameRing.h"

namespace bl {

VulkanBufferFrameRing::VulkanBufferFrameRing()
    : _bufferSize(0)
    , _buffers({})
    , _mappings({})
{
}

VulkanBufferFrameRing::VulkanBufferFrameRing(VulkanBufferFrameRing&& rhs) noexcept
    : _bufferSize(rhs._bufferSize)
    , _buffers(std::move(rhs._buffers))
    , _mappings(rhs._mappings)
{
    rhs._bufferSize = {};
    rhs._buffers = {};
    rhs._mappings = {};
}

VulkanBufferFrameRing::VulkanBufferFrameRing(
    VulkanDevice*               device,
    VkBufferUsageFlags          usage,
    VmaMemoryUsage              memoryUsage,
    VkDeviceSize                size,
    VmaAllocationCreateFlags    flags)
    : _bufferSize(size)
{
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        VmaAllocationInfo allocInfo = {};
        _buffers[i] = VulkanBuffer{device, usage, memoryUsage, size, &allocInfo, true, flags};
        _mappings[i] = allocInfo.pMappedData;
    }
}

VulkanBufferFrameRing::~VulkanBufferFrameRing()
{
}

VulkanBufferFrameRing& VulkanBufferFrameRing::operator=(VulkanBufferFrameRing&& rhs) noexcept
{
    _bufferSize = rhs._bufferSize;
    _buffers = std::move(rhs._buffers);
    _mappings = rhs._mappings;

    rhs._bufferSize = {};
    rhs._buffers = {};
    rhs._mappings = {};

    return *this;
}

VkBuffer VulkanBufferFrameRing::GetBuffer(uint32_t currentFrame) const
{
    return _buffers[currentFrame].Get();
}

void VulkanBufferFrameRing::Upload(std::span<const std::byte> data, uint32_t currentFrame)
{
    const uint32_t uploadSize = static_cast<uint32_t>(data.size());
    const uint32_t size = uploadSize > _bufferSize ? _bufferSize : uploadSize;

    std::memcpy(_mappings[currentFrame], data.data(), size);
    _buffers[currentFrame].Flush(0, size);
}

VkDescriptorBufferInfo VulkanBufferFrameRing::GetDescriptorInfo(uint32_t currentFrame) const
{
    return {
        .buffer = _buffers[currentFrame].Get(),
        .offset = 0,
        .range = _bufferSize
    };
}

} // namespace bl