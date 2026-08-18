#pragma once

#include "Core/Object.h"
#include "Graphics/VulkanImage.h"
#include "Math/Extent.h"
#include "Resources/Resource.h"

namespace bl {

enum class TextureFormat {
    eR,
    eRGB,
    eRGBA,
    eNormals,
    // ... Specular
};

enum class ColorSpace {
    eLinear,
    eSRGB,
};

class Texture : public Resource {
    OBJECT_BOILER_VIRTUAL(Texture, Resource)

protected:
    Extent3D _extent;
    TextureFormat _format;
    ColorSpace _colorSpace;
    std::unique_ptr<VulkanImage> _image;

protected:
    Texture();
    Texture(const Texture& texture);
    Texture(const std::filesystem::path& path);
public:
    virtual ~Texture() = default;

    Extent3D GetExtent3D() const;
    TextureFormat GetFormat() const;
    ColorSpace GetColorSpace() const;
    VulkanImage* GetImage() const;


    static void RegisterClass();
};

} // namespace bl
