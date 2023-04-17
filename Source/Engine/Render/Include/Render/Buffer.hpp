#pragma once

#include "Render/RenderDevice.hpp"

class BLOODLUST_RENDER_API blBuffer
{
public:
    blBuffer(std::shared_ptr<const blRenderDevice> renderDevice, 
        VkDeviceSize size, VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags memoryProperties);
    blBuffer(blBuffer&& other) noexcept;
    blBuffer(blBuffer& other);
    ~blBuffer() noexcept;

    blBuffer& operator=(blBuffer&& rhs) noexcept;
    blBuffer& operator=(blBuffer& rhs);

    VkDeviceSize getSize() const noexcept;
    VmaAllocation getAllocation() const noexcept;
    VkBuffer getBuffer() const noexcept;
    void upload(size_t size, const uint8_t* pData);

private:
    void collapse() noexcept;

    std::shared_ptr<const blRenderDevice> _renderDevice;

    VkDeviceSize    _size;
    VmaAllocation   _allocation;
    VkBuffer        _buffer;
};