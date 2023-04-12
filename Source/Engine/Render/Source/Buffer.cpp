#include "Core/Macros.hpp"
#include "Render/Buffer.hpp"

namespace bl
{

Buffer::Buffer() noexcept
    : m_pRenderDevice(nullptr)
    , m_allocation(VK_NULL_HANDLE)
    , m_buffer(VK_NULL_HANDLE)
    , m_size(0)
{
}

Buffer::Buffer(RenderDevice& renderDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) noexcept
    : Buffer()
{
    m_pRenderDevice = &renderDevice;
    m_size = size;

    const uint32_t graphicsFamilyIndex = m_pRenderDevice->getGraphicsFamilyIndex();
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

    BL_CHECK_NR(
        vmaCreateBuffer(m_pRenderDevice->getAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS,
        "Could not create a vulkan buffer!")
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
    collapse();

    m_pRenderDevice = rhs.m_pRenderDevice;
    m_size = rhs.m_size;
    m_buffer = rhs.m_buffer;
    m_allocation = rhs.m_allocation;

    rhs.collapse();
    return *this;
}

bool Buffer::good() const noexcept
{
    return not m_pRenderDevice || not m_buffer;
}

bool Buffer::upload(size_t size, const uint8_t* pData) noexcept
{
    return true;
}

VmaAllocation Buffer::getAllocation() const noexcept
{
    return m_allocation;
}

VkBuffer Buffer::getBuffer() const noexcept
{
    return m_buffer;
}

VkDeviceSize Buffer::getSize() const noexcept
{
    return m_size;
}

void Buffer::collapse() noexcept
{
    if (not m_pRenderDevice || not m_buffer) return;

    vmaDestroyBuffer(m_pRenderDevice->getAllocator(), m_buffer, m_allocation);

    m_pRenderDevice = nullptr;
    m_size = 0;
    m_buffer = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

} // namespace bl