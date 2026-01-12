#include "VulkanBufferFrameRing.h"

namespace bl {

VulkanBufferFrameRing::VulkanBufferFrameRing()
    : _frameCount(0)
    , _frameSize(0)
    , _alignedFrameSize(0)
    , _bufferWholeSize(0)
    , _buffer({})
    , _usesStagingBuffer(false)
    , _stagingBuffer({})
    , _mapped(nullptr)
{
}

VulkanBufferFrameRing::VulkanBufferFrameRing(VulkanBufferFrameRing&& rhs) noexcept
    : _frameCount(rhs._frameCount)
    , _frameSize(rhs._frameSize)
    , _alignedFrameSize(rhs._alignedFrameSize)
    , _bufferWholeSize(rhs._bufferWholeSize)
    , _buffer(std::move(rhs._buffer))
    , _usesStagingBuffer(rhs._usesStagingBuffer)
    , _stagingBuffer(std::move(rhs._stagingBuffer))
    , _mapped(rhs._mapped)
{
    rhs._frameCount = {};
    rhs._frameSize = {};
    rhs._alignedFrameSize = {};
    rhs._bufferWholeSize = {};
    rhs._buffer = {};
    rhs._usesStagingBuffer = {};
    rhs._stagingBuffer = {};
    rhs._mapped = {};
}

VulkanBufferFrameRing::VulkanBufferFrameRing(
    VulkanDevice*       device,
    VkBufferUsageFlags  usage,
    VmaMemoryUsage      memoryUsage,
    uint32_t            frameCount,
    VkDeviceSize        frameSize,
    bool                mapped,
    bool                dynamicAlignment)
    : _frameSize(frameSize)
    , _frameCount(frameCount)
{

    // Compute the buffer sizings.
    if (dynamicAlignment)
        _alignedFrameSize = device->GetDynamicAlignment(frameSize);
    else
        _alignedFrameSize = frameSize;

    _bufferWholeSize = _alignedFrameSize * _frameCount;

    // Not being mapped implies the buffer is GPU only and requires a staging buffer.
    // Thus the buffer must be a transfer destination.
    if (!mapped) {
        if (!(usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
            Print::Warn("Vulkan buffer frame ring requires TRANSFER_DST_BIT if not CPU mapped.");
        }

        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    // Allocate the buffer.
    VmaAllocationInfo info = {};
    _buffer = VulkanBuffer{device, usage, memoryUsage, _bufferWholeSize, &info, mapped};

    // Set the mapped value if it's host visible.
    if (mapped) {
        _mapped = info.pMappedData;
        _usesStagingBuffer = false;
    } else {

        // Use a staging buffer for CPU -> GPU transfer operations.
        _usesStagingBuffer = true;
        _stagingBuffer = VulkanBuffer{device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, _bufferWholeSize, &info, true, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

        _mapped = info.pMappedData;
    }
}

VulkanBufferFrameRing::~VulkanBufferFrameRing()
{
}

VulkanBufferFrameRing& VulkanBufferFrameRing::operator=(VulkanBufferFrameRing&& rhs) noexcept
{
    _frameCount = rhs._frameCount;
    _frameSize = rhs._frameSize;
    _alignedFrameSize = rhs._alignedFrameSize;
    _bufferWholeSize = rhs._bufferWholeSize;
    _buffer = std::move(rhs._buffer);
    _usesStagingBuffer = rhs._usesStagingBuffer;
    _stagingBuffer = std::move(rhs._stagingBuffer);
    _mapped = rhs._mapped;

    rhs._frameCount = {};
    rhs._frameSize = {};
    rhs._alignedFrameSize = {};
    rhs._bufferWholeSize = {};
    rhs._buffer = {};
    rhs._usesStagingBuffer = {};
    rhs._stagingBuffer = {};
    rhs._mapped = {};

    return *this;
}

uint32_t VulkanBufferFrameRing::GetFrameCount() const
{
    return _frameCount;
}

uint32_t VulkanBufferFrameRing::GetDynamicOffset(uint32_t currentFrame) const
{
    return static_cast<uint32_t>(_alignedFrameSize) * currentFrame;
}

VkDeviceSize VulkanBufferFrameRing::GetWholeSize() const
{
    return _bufferWholeSize;
}

VkBuffer VulkanBufferFrameRing::GetBuffer() const
{
    return _buffer.Get();
}

void VulkanBufferFrameRing::UploadHostVisible(std::span<const std::byte> data, uint32_t currentFrame)
{
    if (_usesStagingBuffer)
        throw std::runtime_error("Cannot perform a host visible upload if using a staging buffer!");

    VkDeviceSize offset = GetDynamicOffset(currentFrame);
    VkDeviceSize size = static_cast<VkDeviceSize>(std::clamp<VkDeviceSize>(data.size(), 0UL, _frameSize));
    std::memcpy(static_cast<char*>(_mapped) + offset, data.data(), size);

    _buffer.Flush(offset, size);
}

void VulkanBufferFrameRing::Upload(VkCommandBuffer cmd, std::span<const std::byte> data, uint32_t currentFrame)
{
    VkDeviceSize offset = GetDynamicOffset(currentFrame);
    VkDeviceSize size = static_cast<VkDeviceSize>(std::clamp<VkDeviceSize>(data.size(), 0UL, _frameSize));
    std::memcpy(static_cast<char*>(_mapped) + offset, data.data(), size);

    if (_usesStagingBuffer) {
        _stagingBuffer.Flush(offset, size);
    } else {
        _buffer.Flush(offset, size);
        return;
    }

    VkBufferCopy region = {};
    region.srcOffset = offset;
    region.dstOffset = offset;
    region.size = size;

    vkCmdCopyBuffer(cmd, _stagingBuffer.Get(), _buffer.Get(), 1, &region);
}

} // namespace bl