#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Math/Vector3.h"
#include "Device.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

class BLUEMETAL_API Image
{
public:

    /// Creates a graphics image on the physical device.
    ///
    ///     @param pDevice Device used to create the image with.
    ///     @param type Vulkan type of image to create. (1D, 2D or 3D and array variants)
    ///     @param format Vulkan format to create the image using.
    ///     @param extent How large to create the image in pixels.
    ///     @param usage Vulkan usage determining where/how this image will work.
    ///     @param aspectMask Vulkan aspects the image will use.
    ///     @param mipLevels How many levels of mip maps to generate.
    ///
    Image(
        GraphicsDevice*     pDevice, 
        VkImageType         type, 
        VkFormat            format, 
        VkExtent3D          extent,  
        VkImageUsageFlags   usage, 
        VkImageAspectFlags  aspectMask,
        uint32_t            mipLevels = 0);

    /// Move Constructor    
    Image(Image&& other);
    
    // Default Destructor
    ~Image();

    /// Returns the image size in pixels at construction.
    Extent3D getExtent();
    
    /// Returns the image format at construction.
    VkFormat getFormat();

    /// Returns the image usage at construction.
    VkImageUsageFlags getUsage();

    /// Returns the vulkan image created at construction. 
    VkImage getHandle();

    /// Returns the default image view created at construction.
    VkImageView getDefaultView();

private:
    GraphicsDevice*     m_pDevice;
    Extent3D            m_extent;
    VkImageType         m_type;
    VkFormat            m_format;
    VkImageUsageFlags   m_usage;
    VkImage             m_image;
    VkImageView         m_imageView;
    VmaAllocation       m_allocation;
};

} // namespace bl