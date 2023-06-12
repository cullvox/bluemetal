#pragma once

#include "Render/RenderDevice.hpp"
#include <vulkan/vulkan.hpp>

class BLUEMETAL_RENDER_API blBuffer
{
public:
    blBuffer( // creates a normal vulkan buffer
        std::shared_ptr<blRenderDevice> renderDevice, 
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags memoryProperties,
        vk::DeviceSize size); 
    blBuffer( // + automatically uploads cpu -> gpu
        std::shared_ptr<blRenderDevice> renderDevice,
        vk::BufferUsageFlags usage,
        vk::DeviceSize size, 
        const void* pData); 
    blBuffer(blBuffer&& other) noexcept;
    blBuffer(blBuffer& other);
    ~blBuffer() noexcept;

    blBuffer& operator=(blBuffer&& rhs) noexcept;
    blBuffer& operator=(blBuffer& rhs);

    vk::DeviceSize getSize() const noexcept;
    vk::Buffer getBuffer() const noexcept;
    VmaAllocation getAllocation() const noexcept;

private:
    void collapse() noexcept;

    std::shared_ptr<const blRenderDevice> _renderDevice;

    vk::DeviceSize    _size;
    vk::Buffer        _buffer;
    VmaAllocation   _allocation;
};