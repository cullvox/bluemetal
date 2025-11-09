#include "Texture2D.h"

#include "qoixx.hpp"
#include "stb_image.h"

namespace bl {

Texture2D::Texture2D(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
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

    if (extension == ".png" || extension == ".jpeg" || extension == ".jpg") {
        DecodeSTBI(buffer, imageData);
    } else if (extension == ".qoi") {
        DecodeQOI(buffer, imageData);
    } else {
        throw std::runtime_error("Invalid texture extension cannot parse image!");
    }

    VkFormat format = VK_FORMAT_UNDEFINED;
    static VkFormat formatConversion[2][2] = {
        { VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8A8_SRGB },
        { VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8A8_UNORM }
    };

    format = formatConversion[(int)GetColorSpace()][(int)GetFormat()];

    _image = std::make_unique<VulkanImage>(system->GetDevice(), VK_IMAGE_TYPE_2D, _extent, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    _image->UploadData(imageData, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Texture2D::Texture2D(ResourceSystem* rs, GraphicsSystem* gs, std::span<std::byte> data)
    : Texture(rs, gs, "")
{
    std::vector<std::byte> imageData;

    if (!DecodeQOI(data, imageData) &&
        !DecodeSTBI(data, imageData))
        throw std::runtime_error("Could not decode image resource!");

    VkFormat format = VK_FORMAT_UNDEFINED;
    static VkFormat formatConversion[2][2] = {
        { VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8A8_SRGB },
        { VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8A8_UNORM }
    };

    format = formatConversion[(int)GetColorSpace()][(int)GetFormat()];

    _image = std::make_unique<VulkanImage>(gs->GetDevice(), VK_IMAGE_TYPE_2D, _extent, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    _image->UploadData(imageData, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

bool Texture2D::DecodeQOI(std::span<std::byte> data, std::vector<std::byte>& out)
{
    using namespace qoixx;

    try {
        auto [pixels, desc] = qoi::decode<std::vector<std::byte>>(data, 4);
        out = pixels;

        _extent = { desc.width, desc.height, 1 };
        _format = TextureFormat::eRGBA;

        switch (desc.colorspace) {
        case qoi::colorspace::linear:
            _colorSpace = ColorSpace::eLinear;
            break;
        case qoi::colorspace::srgb:
            _colorSpace = ColorSpace::eSRGB;
            Print::Warn("Texture encoded using SRGB, this project should use a linear colorspace.");
            break;
        }
    } catch (...) {
        return false;
    }

    return true;
}

bool Texture2D::DecodeSTBI(std::span<std::byte> data, std::vector<std::byte>& out)
{
    int channels = 0, width = 0, height = 0;
    stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), (int)data.size_bytes(), &width, &height, &channels, 4);
    if (!pixels) {
        return false;
    }

    const size_t byteSize = width * height * 4;

    out.resize(byteSize);
    std::memcpy(out.data(), pixels, byteSize);

    stbi_image_free(pixels);
    return true;
}

} // namespace bl