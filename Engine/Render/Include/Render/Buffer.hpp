#pragma once

//===========================//
#include <cstddef>
#include <cstdint>
//===========================//
#include "Render/RenderDevice.hpp"
//===========================//

namespace bl
{

class BLOODLUST_API Buffer
{
public:
    Buffer();
    Buffer(RenderDevice& renderDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
    ~Buffer();

    Buffer& operator=(Buffer& rhs) noexcept;

    void            Upload(size_t dataSize, const uint8_t* pData);
    VmaAllocation   GetAllocation() const noexcept;
    VkBuffer        GetBuffer() const noexcept;
    VkDeviceSize    GetSize() const noexcept;
private:
    RenderDevice*   m_pRenderDevice;
    VmaAllocation   m_allocation;
    VkBuffer        m_buffer;
    VkDeviceSize    m_size;
};

} /* namespace bl */