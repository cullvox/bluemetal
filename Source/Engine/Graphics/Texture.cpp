#include "Core/Print.h"
#include "Graphics/Mesh.h"
#include "Texture.h"

#include <spng.h>
#include "qoixx.hpp"

namespace bl {

Texture::Texture(const nlohmann::json& data)
    : Resource(data) {}

Texture::~Texture() {}

void Texture::Load() {
    std::filesystem::path path = GetPath();
    
    // Readin the image file to a buffer.
    std::ifstream file{path, std::ios::binary};
    
    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer{size};
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    buffer.resize(file.tellg());

    // Use the extension to determine the file type.
    std::string extension = path.extension();

    // Transform the extension to only lowercase.
    std::transform(extension.begin(), extension.end(), extension.begin(), 
        [](unsigned char c){ return std::tolower(c); });

    if (extension == ".png") {
        DecodePNG(buffer);
    } else if (extension == ".qoi") {
        DecodeQOI(buffer);
    } else {
        blError("Invalid texture extension cannot parse image: {}", path.c_str());
        return;
    }

    Resource::Load(); // Set the resource state as loaded.
}

void Texture::Unload() {
    _imageData.resize(0);
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

void Texture::DecodePNG(const std::vector<std::byte>& data) {
    spng_ctx* ctx = spng_ctx_new(0);
    spng_set_png_buffer(ctx, data.data(), data.size());

    size_t decodedSize = 0;
    spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &decodedSize);

    _imageData.resize(decodedSize);
    spng_decode_image(ctx, _imageData.data(), _imageData.size(), SPNG_FMT_RGBA8, 0);
    _format = TextureFormat::eRGBA;

    spng_ihdr header = {};
    spng_get_ihdr(ctx, &header);

    _extent = {header.width, header.height};

    spng_ctx_free(ctx);
}

void Texture::DecodeQOI(const std::vector<std::byte>& data) {
    using namespace qoixx;

    const auto [actual, desc] = qoi::decode<std::vector<std::byte>>(data, 4);
    _imageData = actual;

    _extent = {desc.width, desc.height};
    _format = TextureFormat::eRGBA;
    
    switch (desc.colorspace) {
        case qoi::colorspace::linear: _colorSpace = TextureColorSpace::eLinear;
        case qoi::colorspace::srgb: _colorSpace = TextureColorSpace::eSRGB;
    }
}

} // namespace bl