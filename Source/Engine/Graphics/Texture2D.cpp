#include "Texture2D.h"
#include "Graphics/Texture.h"
#include "Resource/Resource.h"

namespace bl {
    
Texture2D::Texture2D(const nlohmann::json& data, VulkanDevice* device)
    : Texture(data)
    , _device(device) {}

void Texture2D::Load() {
    Texture::Load();

    Texture::Unload();
    Resource::Load();
}

void Texture2D::Unload() {
    _image.Destroy();
    Resource::Unload();
}

} // namespace bl