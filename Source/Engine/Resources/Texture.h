#pragma once

#include "Resources/Resource.h"
#include "Vulkan.h"

namespace bl {

/**
 * @enum TextureFormat
 * @brief Pixel formats that are currently supported for a texture object.
 */
enum class TextureFormat {
    eRGB,
    eRGBA,
    eNormals,
    // ... Specular
};

/**
 * @enum ColorSpace
 * @brief Texture color spaces that are currently supported.
 */
enum class ColorSpace {
    eLinear, // Linear is preferable for almost any kind of texture.
    eSRGB,
};

/**
 * @class Texture
 * @brief Generic texture loading resource interface.
 *
 * This is a generic texture loader and will load the file generically of type, size, or color space.
 * It's useful for further basic textures to inherit from this or for building other resources.
 * Texture resources are probably only going to be loaded for a short period of time until the file
 * gets uploaded to the GPU and the resource manager will automatically unload this resource.
 */
class Texture : public Resource {
    CLASS_OBJECT_VIRTUAL(Texture, Resource)
public:

    /**
     * @brief Destroy the Texture resource.
     */
    virtual ~Texture() = 0;

    /**
     * @brief Get the extent (width and height) of the texture.
     * @return VkExtent2D containing the texture dimensions.
     */
    VkExtent3D GetExtent() const;

    /**
     * @brief Get the pixel format of the texture.
     * @return TextureFormat enum value.
     */
    TextureFormat GetFormat() const;

    /**
     * @brief Get the color space of the texture.
     * @return ColorSpace enum value.
     */
    ColorSpace GetColorSpace() const;

    /**
     * @brief Get the raw image data of the texture.
     * @return std::span containing the image bytes.
     */
    std::span<const std::byte> GetImageData() const;

private:
    void DecodePNG(const std::vector<std::byte>& buffer);
    void DecodeQOI(const std::vector<std::byte>& buffer);

    VkExtent2D _extent;
    TextureFormat _format;
    ColorSpace _colorSpace;
    std::vector<std::byte> _imageData;
};

} // namespace bl
