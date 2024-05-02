#pragma once

#include "Device.h"

namespace bl 
{

/** @brief Creates a graphics image on the physical device. */
class Image 
{
public:
    Image(Device* device, VkImageType type, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, uint32_t mipLevels = 0); /** @brief Constructor */
    Image(Image& copy); /** @brief Copy constructor */
    Image(Image&& move); /** @brief Move constructor */
    ~Image(); /** @brief Destructor */

    Image& operator=(Image& rhs); /** @brief Assign copy  */ 
    Image& operator=(Image&& rhs); /** @brief Assign move */

    VkImageType GetType() const; /** @brief Returns the image type. */
    VkExtent3D GetExtent() const; /** @brief Returns the image size in pixels at construction. */
    VkFormat GetFormat() const; /** @brief Returns the image format at construction. */
    VkImageUsageFlags GetUsage() const; /** @brief Returns the image usage at construction. */
    VkImage Get() const; /** @brief Returns the vulkan image created at construction.  */
    VkImageView GetView() const; /** @brief Returns the default image view created at construction. */

private:
    void CreateImage();
    void Copy(Image& other);

    Device* _device;
    VkExtent3D _extent;
    VkImageType _type;
    VkFormat _format;
    VkImageUsageFlags _usage;
    VkImageAspectFlags _aspectMask;
    uint32_t _mipLevels;
    VkImage _image;
    VkImageView _imageView;
    VmaAllocation _allocation;
};

} // namespace bl
