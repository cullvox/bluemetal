#pragma once

#include "Device.h"

namespace bl {

/// @brief Creates a graphics image on the physical device.
class Image {
public:
    /// @brief Default Constructor
    Image();

    /// @brief Image Constructor
    /// @param[in] device Device to contruct the image from.
    /// @param[in] type Type of image to create.
    /// @param[in] extent The extent in pixels of the image.
    /// @param[in] usage What the image is used for in api.
    /// @param[in] viewAspectMask The default image view aspect mask.
    /// @param[in] mipLevels How many mipmap levels will this image have. 
    Image(Device*           device, 
        VkImageType         type, 
        VkExtent3D          extent, 
        VkFormat            format, 
        VkImageUsageFlags   usage, 
        VkImageAspectFlags  viewAspectMask, 
        VkImageLayout       initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, 
        uint32_t            mipLevels = 0);

    /// @brief Move Constructor
    /// @param[inout] image The other image to move it's data into this new object. 
    Image(Image&& image);

    /// @brief Default Destructor
    ~Image(); 

public:
    /// @brief Move Assign Operator 
    /// @param[inout] rhs The other image to move it's data into this new object. 
    Image& operator=(Image&& rhs); /** @brief Assign move */

public:
    /// @brief GetType 
    /// @returns The type of image this was created as.
    VkImageType GetType() const;

    /// @brief GetExtent
    /// @returns  Returns the image size in pixels at construction.
    VkExtent3D GetExtent() const;

    /// @brief GetFormat
    /// @returns Returns the current image format.
    VkFormat GetFormat() const;
    
    /// @brief GetUsage
    /// @returns Returns the image usage at construction. 
    VkImageUsageFlags GetUsage() const; 

    /// @brief GetLayout
    /// @return Returns the current image layout.
    VkImageLayout GetLayout() const;

    /// @brief Get
    /// @return Returns the underlying Vulkan image from construction.
    VkImage Get() const;

    /// @brief GetView
    /// @return Returns the default image view created at construction.
    VkImageView GetView() const;

public:
    /// @brief Transitions the image from the previous layout to another new one.
    /// @param layout[in] New layout to transition the image into.  
    /// @param format[in] A new format to set the image to, leave empty to use the current format.
    void Transition(VkImageLayout layout);

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
    VkImageLayout _layout;
    VkImage _image;
    VkImageView _imageView;
    VmaAllocation _allocation;
};

} // namespace bl
