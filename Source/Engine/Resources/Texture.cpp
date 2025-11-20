
#include "Texture.h"
#include "Core/Print.h"

namespace bl {

Texture::Texture(ResourceSystem* resourceSystem, System* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
{
}

Extent3D Texture::GetExtent3D() const
{
    return _extent;
}

TextureFormat Texture::GetFormat() const
{
    return _format;
}

ColorSpace Texture::GetColorSpace() const
{
    return _colorSpace;
}

VulkanImage* Texture::GetImage() const
{
    return _image.get();
}

} // namespace bl