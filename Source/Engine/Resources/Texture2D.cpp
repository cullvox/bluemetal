#include "Texture2D.h"
#include "qoixx.hpp"


namespace bl 
{
    
namespace vk 
{

VkExtent3D Make3D(VkExtent2D ex, uint32_t depth = 1) {
    return VkExtent3D{ex.width, ex.height, depth};
}

}

Texture2D::Texture2D(const std::filesystem::path& path)
    : Texture()
{
    std::ifstream file{path, std::ios::binary};

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

    if (extension == ".png" || extension == ".jpg") 
    {
        throw std::runtime_error("Cannot load png images in engine!");
    } 
    else if (extension == ".qoi") 
    {
        using namespace qoixx;

        try {
            const auto [actual, desc] = qoi::decode<std::vector<std::byte>>(data, 4);

            _imageData = std::move(actual);
            _extent = { desc.width, desc.height };
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
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Could not decode a QOI texture.")
        }
    } 
    else 
    {
        throw std::runtime_error("Invalid texture extension cannot parse image!");
    }

    VkFormat format = VK_FORMAT_UNDEFINED;
    static VkFormat formatConversion[2][2] = {
        { VK_FORMAT_R8G8B8_SRGB, VK_FORMAT_R8G8B8A8_SRGB },
        { VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8A8_UNORM }
    };

    format = formatConversion[(int)GetColorSpace()][(int)GetFormat()];

    _image = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, vk::Make3D(GetExtent()), format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    _image->UploadData(buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

Texture2D::~Texture2D() 
{
}

VulkanImage* Texture2D::GetImage() 
{
    return _image.get();
}

} // namespace bl