#pragma once

#include "Resources/Resource.h"
#include "Graphics/VulkanImage.h"

namespace bl 
{

enum class TextureFormat 
{
    eRGB,
    eRGBA,
    eNormals,
    // ... Specular
};

enum class ColorSpace 
{
    eLinear, // Linear is preferable for almost any kind of texture.
    eSRGB,
};

class Texture : public Resource 
{
protected:
    VkExtent3D _extent;
    TextureFormat _format;
    ColorSpace _colorSpace;
    std::unique_ptr<VulkanImage> _image;

public:
    Texture(ResourceSystem* resourceSystem, System* system, const std::filesystem::path& path)
        : Resource(resourceSystem, system, path)
    {
    }
    virtual ~Texture() = default;

    VkExtent3D GetExtent3D() const { return _extent; };
    TextureFormat GetFormat() const { return _format; };
    ColorSpace GetColorSpace() const { return _colorSpace; };
    VulkanImage* GetImage() const { return _image.get(); }

};

} // namespace bl
