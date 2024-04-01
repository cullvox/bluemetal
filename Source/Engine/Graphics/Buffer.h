#pragma once

#include "Device.h"

namespace bl {

class BLUEMETAL_API GfxBuffer
{
public:

    struct CreateInfo 
    {
        VkBufferUsageFlags      usage;                      /** @brief Vulkan memory usage, specifing buffer type. */
        VkMemoryPropertyFlags   memoryProperties;           /** @brief Vulkan memory properies, how this buffer will be used. */
        VkDeviceSize            size;                       /** @brief Size of your buffer in bytes. */
        VmaAllocationInfo*      pAllocationInfo = nullptr;  /** @brief Output value, additional allocation info if needed. */
        bool                    mapped = false;             /** @brief Automatically have your buffer mapped to a pointer. */
    };

    /** @brief Constructor */
    GfxBuffer(std::shared_ptr<GfxDevice> device, const CreateInfo& createInfo);

    /** @brief Destructor */
    ~GfxBuffer();

public:

    /** @brief Returns the size of the buffer in bytes. */
    VkDeviceSize getSize() const;

    /** @brief Returns the underlying vulkan buffer. */
    VkBuffer getBuffer() const;

    /** @brief Returns the underlying VMA allocation. */
    VmaAllocation getAllocation() const;

    bool upload() const;

    /** @brief Create an exact copy of this buffer.  */
    GfxBuffer copy();

private:
    void createBuffer(const CreateInfo& createInfo);

    std::shared_ptr<GfxDevice>  _device;
    VkDeviceSize                _size;
    VkBuffer                    _buffer;
    VmaAllocation               _allocation;
};

} // namespace bl
