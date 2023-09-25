#pragma once

#include "Math/Vector3.h"
#include "Device.h"

namespace bl {

struct BLUEMETAL_API ImageCreateInfo {
    GraphicsDevice*     pDevice,       /** @brief pDevice Device used to create the image with. */
    VkImageType         type,          /** @brief type Vulkan type of image to create. (1D, 2D or 3D and array variants) */
    VkFormat            format,        /** @brief format Vulkan format to create the image using. */
    VkExtent3D          extent,        /** @brief extent How large to create the image in pixels. */
    VkImageUsageFlags   usage,         /** @brief usage Vulkan usage determining where/how this image will work. */
    VkImageAspectFlags  aspectMask,    /** @brief aspectMask Vulkan aspects the image will use. */
    uint32_t            mipLevels = 0  /** @brief mipLevels How many levels of mip maps to generate. */
};

/** @brief Creates a graphics image on the physical device. */
class BLUEMETAL_API Image {
public:

    /** @brief Default Constrctor */
    Image();

    /** @brief Create Constructor */
    Image(const ImageCreateInfo& createInfo);

    /** @brief Move Constructor */    
    Image(Image&& other);
    
    /** @brief Default Destructor */
    ~Image();

    /** @brief Move Operator */
    Image& operator=(Image&& rhs) noexcept;

    /** @brief Creates this image. */
    bool create(const ImageCreateInfo& createInfo) noexcept;

    /** @brief Destroys this image. */
    void destroy() noexcept;

    /** @brief Returns true if the image was created. */
    bool isCreated() const noexcept;

public:

    /** @brief Returns the image size in pixels at construction. */
    [[nodiscard]] Extent3D getExtent() const noexcept;
    
    /** @brief Returns the image format at construction. */
    [[nodiscard]] VkFormat getFormat() const noexcept;

    /** @brief Returns the image usage at construction. */
    [[nodiscard]] VkImageUsageFlags getUsage() const noexcept;

    /** @brief Returns the vulkan image created at construction.  */
    [[nodiscard]] VkImage getHandle() const noexcept;

    /** @brief Returns the default image view created at construction. */
    [[nodiscard]] VkImageView getDefaultView() const noexcept;

private:
    GraphicsDevice*     _pDevice;
    Extent3D            _extent;
    VkImageType         _type;
    VkFormat            _format;
    VkImageUsageFlags   _usage;
    VkImage             _image;
    VkImageView         _imageView;
    VmaAllocation       _allocation;
};

} // namespace bl
