#include "Texture2D.h"

#include "Graphics/GraphicsSystem.h"

#include "qoixx.hpp"
#include "stb_image.h"

namespace bl {

bool DecodeQOI(std::span<std::byte> data, std::vector<std::byte>& out, Extent2D& extent, TextureFormat& format, ColorSpace& colorSpace)
{
    using namespace qoixx;

    try {
        auto [pixels, desc] = qoi::decode<std::vector<std::byte>>(data, 4);
        out = pixels;

        extent = { desc.width, desc.height };
        format = TextureFormat::eRGBA;
        colorSpace = ColorSpace::eSRGB;
    } catch (...) {
        return false;
    }

    return true;
}

bool DecodeSTBI(std::span<std::byte> data, std::vector<std::byte>& out, Extent2D& extent, TextureFormat& format, ColorSpace& colorSpace)
{
    int channels = 0, width = 0, height = 0;
    stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), static_cast<int>(data.size()), &width, &height, &channels, 4);
    if (!pixels) {
        return false;
    }

    extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    format = TextureFormat::eRGBA; // we always get four channels.
    colorSpace = ColorSpace::eSRGB; // stb_image.h does not determine color space.
    const size_t byteSize = width * height * 4;

    out.resize(byteSize);
    std::memcpy(out.data(), pixels, byteSize);

    stbi_image_free(pixels);
    return true;
}

Texture2D::Texture2D(ResourceSystem& rs, GraphicsSystem* gs)
    : Texture(rs, gs, {})
{
    // Only used in child texture classes.
}

Texture2D::Texture2D(ResourceSystem& resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Texture(resourceSystem, system, path)
{
    std::ifstream file { path, std::ios::binary };

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer { size };
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    buffer.resize(file.tellg());

    // Use the extension to determine the file type.
    std::string extension = path.extension().string();

    // Transform the extension to only lowercase.
    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char c) { return std::tolower(c); });

    std::vector<std::byte> imageData;

    Extent2D extent {};
    if (extension == ".png" || extension == ".jpeg" || extension == ".jpg") {
        if (!DecodeSTBI(buffer, imageData, extent, _format, _colorSpace)) {
            throw std::runtime_error("Could not decode an image! STB");
        }
    } else if (extension == ".qoi") {
        if (!DecodeQOI(buffer, imageData, extent, _format, _colorSpace)) {
            throw std::runtime_error("Could not decode an image! QOI");
        }
    } else {
        throw std::runtime_error("Invalid texture extension cannot parse image!");
    }

    _extent = extent.To3D();
    VkExtent3D vulkanExtent = { _extent.width, _extent.height, _extent.depth };

    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    switch (_format) {
    case TextureFormat::eRGB:
        imageFormat = VK_FORMAT_R8G8B8_SRGB;
        break;
    case TextureFormat::eRGBA:
        imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
        break;
    default:
        throw std::runtime_error("Invalid texture format!");
    }

    _image = std::make_unique<VulkanImage>(system->GetDevice(), VK_IMAGE_TYPE_2D, vulkanExtent, imageFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, true);
    _image->UploadData(imageData, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Texture2D::Texture2D(ResourceSystem& rs, GraphicsSystem* gs, std::span<std::byte> data)
    : Texture(rs, gs, "")
{
    std::vector<std::byte> imageData;

    Extent2D extent {};
    if (!DecodeQOI(data, imageData, extent, _format, _colorSpace) && !DecodeSTBI(data, imageData, extent, _format, _colorSpace)) {
        throw std::runtime_error("Could not decode image resource!");
    }

    _extent = extent.To3D();
    VkExtent3D vulkanExtent = { _extent.width, _extent.height, _extent.depth };

    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    switch (_format) {
    case TextureFormat::eRGB:
        imageFormat = VK_FORMAT_R8G8B8_SRGB;
        break;
    case TextureFormat::eRGBA:
        imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
        break;
    default:
        throw std::runtime_error("Invalid texture format!");
    }

    _image = std::make_unique<VulkanImage>(gs->GetDevice(), VK_IMAGE_TYPE_2D, vulkanExtent, imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    _image->UploadData(imageData, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Texture2D::Texture2D(ResourceSystem& rs, GraphicsSystem* gs, const std::span<const std::byte> pixels, TextureFormat format, Extent2D extent)
    : Texture(rs, gs, "")
{
    _format = format;
    _extent = extent.To3D();

    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    switch (format) {
    case TextureFormat::eRGB:
        imageFormat = VK_FORMAT_R8G8B8_SRGB;
        break;
    case TextureFormat::eRGBA:
        imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
        break;
    default:
        throw std::runtime_error("Invalid texture format!");
    }

    VkExtent3D vulkanExtent = { _extent.width, _extent.height, _extent.depth };

    _image = std::make_unique<VulkanImage>(gs->GetDevice(), VK_IMAGE_TYPE_2D, vulkanExtent, imageFormat, VK_IMAGE_USAGE_SAMPLED_BIT, true);
    _image->UploadData(pixels, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    assert(_image->GetLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

} // namespace bl