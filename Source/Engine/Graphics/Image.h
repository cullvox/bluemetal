#pragma once

#include "Math/Vector3.h"
#include "Device.h"

namespace bl {


/** @brief Creates a graphics image on the physical device. */
class BLUEMETAL_API GfxImage {
public:

    struct CreateInfo 
    {
        VkImageType         type;          /** @brief type Vulkan type of image to create. (1D, 2D or 3D and array variants) */
        VkFormat            format;        /** @brief format Vulkan format to create the image using. */
        VkExtent3D          extent;        /** @brief extent How large to create the image in pixels. */
        VkImageUsageFlags   usage;         /** @brief usage Vulkan usage determining where/how this image will work. */
        VkImageAspectFlags  aspectMask;    /** @brief aspectMask Vulkan aspects the image will use. */
        uint32_t            mipLevels = 0; /** @brief mipLevels How many levels of mip maps to generate. */
    };

    /** @brief Constructor */
    GfxImage(std::shared_ptr<GfxDevice> device, const CreateInfo& createInfo);

    /** @brief Destructor */
    ~GfxImage();

public:

    /** @brief Returns the image size in pixels at construction. */
    Extent3D getExtent() const;
    
    /** @brief Returns the image format at construction. */
    VkFormat getFormat() const;

    /** @brief Returns the image usage at construction. */
    VkImageUsageFlags getUsage() const;

    /** @brief Returns the vulkan image created at construction.  */
    VkImage get() const;

    /** @brief Returns the default image view created at construction. */
    VkImageView getDefaultView() const;

private:
    void createImage(const CreateInfo& createInfo);

    std::shared_ptr<GfxDevice>  _device;
    Extent3D                    _extent;
    VkImageType                 _type;
    VkFormat                    _format;
    VkImageUsageFlags           _usage;
    VkImage                     _image;
    VkImageView                 _imageView;
    VmaAllocation               _allocation;
};

} // namespace bl
