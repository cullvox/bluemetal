#pragma once

#include "Render/RenderDevice.hpp"

namespace bl
{

class BLOODLUST_API Buffer
{
public:
    Buffer() noexcept;
    Buffer(RenderDevice& renderDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) noexcept;
    ~Buffer() noexcept;

    Buffer& operator=(Buffer&& rhs) noexcept;

    VmaAllocation getAllocation() const noexcept;
    VkBuffer getBuffer() const noexcept;
    VkDeviceSize getSize() const noexcept;
    bool good() const noexcept;

    bool upload(size_t size, const uint8_t* pData) noexcept;

private:
    void collapse() noexcept;

    RenderDevice*   m_pRenderDevice;
    VkDeviceSize    m_size;
    VmaAllocation   m_allocation;
    VkBuffer        m_buffer;
};

} /* namespace bl */