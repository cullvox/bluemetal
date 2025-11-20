#pragma once

#include "Graphics/VulkanImage.h"
#include "Math/Extent.h"
#include "Resources/Resource.h"

namespace bl {

enum class TextureFormat {
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
protected:
    Extent3D _extent;
    TextureFormat _format;
    ColorSpace _colorSpace;
    std::unique_ptr<VulkanImage> _image;

public:
    Texture(ResourceSystem* resourceSystem, System* system, const std::filesystem::path& path);
    virtual ~Texture() = default;

    Extent3D GetExtent3D() const;
    TextureFormat GetFormat() const;
    ColorSpace GetColorSpace() const;
    VulkanImage* GetImage() const;
};

} // namespace bl
