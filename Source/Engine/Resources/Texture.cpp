
#include "Texture.h"
#include "Core/Print.h"

namespace bl {

Texture::Texture()
    : Resource()
{
}

Texture::Texture(const Texture& texture)
    : _extent(texture._extent)
    , _format(texture._format)
    , _colorSpace(texture._colorSpace)
{
    throw std::runtime_error("ToDO");
}

Texture::Texture(const std::filesystem::path& path)
    : Resource(path)
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

void Texture::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Texture", "Resource", nullptr);
}

} // namespace bl