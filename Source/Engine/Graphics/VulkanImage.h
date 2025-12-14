#pragma once

#include <span>
#include <vector>

#include "Vulkan.h"

namespace bl {

class VulkanDevice;

/// @brief Creates a graphics image on the physical device.
class VulkanImage {
    VulkanDevice* _device;
    VkExtent3D _extent;
    VkImageType _type;
    VkFormat _format;
    VkImageUsageFlags _usage;
    uint32_t _mipLevels;
    VkSampleCountFlagBits _samples;
    VkImageLayout _layout;
    VkImage _image;
    VkImageView _defaultView;
    std::vector<VkImageView> _views;
    VmaAllocation _allocation;

public:
    /// @brief Default Constructor
    VulkanImage();

    /// @brief Image Constructor
    /// @param[in] device Device to contruct the image from.
    /// @param[in] type Type of image to create.
    /// @param[in] extent The extent in pixels of the image.
    /// @param[in] format The format for pixel storage and data representation.
    /// @param[in] usage What the image is used for in api.
    /// @param[in] viewAspectMask The default image view aspect mask.
    /// @param[in] mipLevels How many mipmap levels will this image have.
    VulkanImage(
        VulkanDevice* device,
        VkImageType type,
        VkExtent3D extent,
        VkFormat format,
        VkImageUsageFlags usage,
        uint32_t mipLevels = 1,
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

    /// @brief Move Constructor
    /// @param[inout] image The other image to move it's data into this new object.
    VulkanImage(VulkanImage&& image);

    /// @brief Default Destructor
    ~VulkanImage();

    /// @brief Move Assign Operator
    /// @param[inout] rhs The other image to move it's data into this new object.
    VulkanImage& operator=(VulkanImage&& rhs);

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
    VkImageView GetDefaultView() const;

    VkImageView CreateView(VkImageAspectFlags viewAspectMask, uint32_t mipLevels = 1);
    void DestroyViews();

    /// @brief Uploads image data into a vulkan image.
    /// @param data The data to upload to the GPU.
    void UploadData(const std::span<const std::byte> data, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    /// @brief Transitions the image from the previous layout to another new one.
    /// @param cmd Command buffer to write the image transition command to.
    /// @param layout[in] New layout to transition the image into.
    void Transition(VkCommandBuffer cmd, VkImageLayout layout);

    /// @brief Transitions the image from the previous layout to another new one.
    /// Immediately submits a command buffer to the graphics card.
    /// @param layout[in] New layout to transition the image into.
    void Transition(VkImageLayout layout);
};

} // namespace bl
