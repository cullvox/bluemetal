#pragma once

#include "Precompiled.h"
#include "Texture.h"

namespace bl {

class GraphicsSystem;

// Textures are all QOI format, probably
class Texture2D : public Texture {
    VulkanDevice* _device;

public:
    Texture2D(ResourceSystem* rs, GraphicsSystem* gs, const std::filesystem::path& path);

    /**
     * @brief Loads a texture resource from memory as an image format.
     *
     * @param[in] rs The resource system object "owning" this resource.
     * @param[in] gs The graphics system which is used when creating the texture.
     * @param[in] data The image data, can be raw pixels or an image file yet to be loaded.
     * @param[in] dataAsPixels
     */
    Texture2D(ResourceSystem* rs, GraphicsSystem* gs, std::span<std::byte> imageData);

    /**
     * @brief Loads a texture resource from from memory as raw pixel data.
     *
     * @param[in] rs The resource system object "owning" this resource.
     * @param[in] gs The graphics system which is used when creating the texture.
     * @param[in] data Raw pixel data uploaded to the image buffer.
     * @param[in] format Format the pixels are in, not all formats are supported, check the physical device.
     * @param[in] extent Pixel count in width and height.
     */
    Texture2D(ResourceSystem* rs, GraphicsSystem* gs, const std::span<const std::byte> pixels, TextureFormat format, Extent2D extent);

    ~Texture2D() = default;
};

} // namespace bl