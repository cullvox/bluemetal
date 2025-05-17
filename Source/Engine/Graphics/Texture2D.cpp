#include "Texture2D.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Graphics/VulkanImage.h"
#include "Resource/Resource.h"

namespace bl {
    
namespace vk {

VkExtent3D Make3D(VkExtent2D ex, uint32_t depth = 1) {
    return VkExtent3D{ex.width, ex.height, depth};
}

}

Texture2D::Texture2D(const nlohmann::json& data, VulkanDevice* device)
    : Texture(data)
    , _device(device) {}

Texture2D::~Texture2D() {}

void Texture2D::Load() {
    Texture::Load();

    VkFormat format = VK_FORMAT_UNDEFINED;

    static VkFormat formatConversion[2][2] = {
        {
            VK_FORMAT_R8G8B8_SRGB,
            VK_FORMAT_R8G8B8A8_SRGB
        },
        {
            VK_FORMAT_R8G8B8_UNORM,
            VK_FORMAT_R8G8B8A8_UNORM
        }
    };

    format = formatConversion[(int)GetColorSpace()][(int)GetFormat()];

    _image = VulkanImage{
        _device, 
        VK_IMAGE_TYPE_2D, 
        vk::Make3D(GetExtent()),
        format,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT};

    _image.UploadData(GetImageData(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    Texture::Unload();
    Resource::Load();
}

void Texture2D::Unload() {
    _image.Destroy();
    Resource::Unload();
}

VulkanImage* Texture2D::GetImage() {
    return &_image;
}

} // namespace bl