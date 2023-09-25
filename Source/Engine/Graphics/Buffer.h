#pragma once

#include "Device.h"

namespace bl {

struct BufferCreateInfo {
    GraphicsDevice*         pDevice;                    /** @brief Graphics device to create the buffer on. */
    VkBufferUsageFlags      usage;                      /** @brief Vulkan memory usage, specifing buffer type. */
    VkMemoryPropertyFlags   memoryProperties;           /** @brief Vulkan memory properies, how this buffer will be used. */
    VkDeviceSize            size;                       /** @brief Size of your buffer in bytes. */
    VmaAllocationInfo*      pAllocationInfo = nullptr;  /** @brief Output value, additional allocation info if needed. */
    bool                    mapped = false;             /** @brief Automatically have your buffer mapped to a pointer. */
};

class BLUEMETAL_API Buffer
{
public:

    /** @brief Default Constructor */
    Buffer();

    /** @brief Move Constructor */
    Buffer(Buffer&& rhs);

    /** @brief Create Constructor */
    Buffer(const BufferCreateInfo& info);

    /** @brief Default Destructor */
    ~Buffer();
    
    /** @brief Move Operator */
    Buffer& operator=(Buffer&& rhs) noexcept;

    /** @brief Create this buffer. */
    [[nodiscard]] bool create(const BufferCreateInfo& info) noexcept;

    /** @brief Destroys this buffer. */
    void destroy() noexcept;

    /** @brief Returns true if the buffer was created. */
    [[nodiscard]] bool isCreated() const noexcept;

public:

    /** @brief Returns the size of the buffer in bytes. */
    [[nodiscard]] VkDeviceSize getSize() const noexcept;

    /** @brief Returns the underlying vulkan buffer. */
    [[nodiscard]] VkBuffer getBuffer() const noexcept;

    /** @brief Returns the underlying VMA allocation. */
    [[nodiscard]] VmaAllocation getAllocation() const noexcept;

    /** @brief Create an exact copy of a buffer on this one, destroys any previous buffers. */
    [[nodiscard]] bool copyFrom(const Buffer& copy);

private:
    GraphicsDevice* _pDevice;
    VkDeviceSize    _size;
    VkBuffer        _buffer;
    VmaAllocation   _allocation;
};

} // namespace bl
