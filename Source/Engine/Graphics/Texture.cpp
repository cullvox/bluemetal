#include "qoixx.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Core/Print.h"
#include "Texture.h"

namespace bl {

Texture::Texture(ResourceManager* manager, const nlohmann::json& data)
    : Resource(manager, data) 
{
}

Texture::~Texture() 
{
}

void Texture::Load() 
{
    std::filesystem::path path = GetFilePath();

    // Readin the image file to a buffer.
    std::ifstream file{path, std::ios::binary};

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer{size};
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    buffer.resize(file.tellg());

    // Use the extension to determine the file type.
    std::string extension = path.extension().string();

    // Transform the extension to only lowercase.
    std::transform(extension.begin(), extension.end(), extension.begin(), 
        [](unsigned char c){ return std::tolower(c); });

    if (extension == ".png" || extension == ".jpg") 
    {
        throw std::runtime_error("Cannot load png images in engine!");
    } 
    else if (extension == ".qoi") 
    {
        DecodeQOI(buffer);
    } 
    else 
    {
        Log::Error("Invalid texture extension cannot parse image: {}", path.string());
        return;
    }

    Resource::Load(); // Set the resource state as loaded.
}

void Texture::Unload() 
{
    _imageData.resize(0);
    Resource::Unload();
}

VkExtent2D Texture::GetExtent() const {
    return _extent;
}

TextureFormat Texture::GetFormat() const {
    return _format;
}

TextureColorSpace Texture::GetColorSpace() const {
    return _colorSpace;
}

std::span<const std::byte> Texture::GetImageData() const {
    return _imageData;
}

void Texture::DecodeQOI(const std::vector<std::byte>& data) {
    using namespace qoixx;

    const auto [actual, desc] = qoi::decode<std::vector<std::byte>>(data, 4);
    _imageData = actual;

    _extent = {desc.width, desc.height};
    _format = TextureFormat::eRGBA;
    
    switch (desc.colorspace) {
        case qoi::colorspace::linear: _colorSpace = TextureColorSpace::eLinear; break;
        case qoi::colorspace::srgb: _colorSpace = TextureColorSpace::eSRGB; break;
    }
}

} // namespace bl