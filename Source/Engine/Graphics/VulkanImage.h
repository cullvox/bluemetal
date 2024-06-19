#pragma once

#include "VulkanDevice.h"
#include "vulkan/vulkan_core.h"

namespace bl {

/// @brief Creates a graphics image on the physical device.
class VulkanImage {
public:
    /// @brief Default Constructor
    VulkanImage();

    /// @brief Image Constructor
    /// @param[in] device Device to contruct the image from.
    /// @param[in] type Type of image to create.
    /// @param[in] extent The extent in pixels of the image.
    /// @param[in] usage What the image is used for in api.
    /// @param[in] viewAspectMask The default image view aspect mask.
    /// @param[in] mipLevels How many mipmap levels will this image have. 
    VulkanImage(
        VulkanDevice*       device, 
        VkImageType         type, 
        VkExtent3D          extent, 
        VkFormat            format, 
        VkImageUsageFlags   usage, 
        VkImageAspectFlags  viewAspectMask, 
        VkImageLayout       initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, 
        uint32_t            mipLevels = 1);

    /// @brief Move Constructor
    /// @param[inout] image The other image to move it's data into this new object. 
    VulkanImage(VulkanImage&& image);

    /// @brief Default Destructor
    ~VulkanImage();

public:
    /// @brief Move Assign Operator 
    /// @param[inout] rhs The other image to move it's data into this new object. 
    VulkanImage& operator=(VulkanImage&& rhs);

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

    /// @brief Destroys the Vulkan image freeing up GPU memory.
    void Destroy();

public:
    void UploadData(std::span<std::byte> data);


    /// @brief Transitions the image from the previous layout to another new one.
    /// @brief Command buffer to write the image transition command to.
    /// @param layout[in] New layout to transition the image into.
    void Transition(VkCommandBuffer cmd, VkImageLayout layout);

    /// @brief Transitions the image from the previous layout to another new one.
    /// Immediately submits a command buffer to the graphics card.
    /// @param layout[in] New layout to transition the image into.
    void Transition(VkImageLayout layout);

private:
    VulkanDevice* _device;
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
