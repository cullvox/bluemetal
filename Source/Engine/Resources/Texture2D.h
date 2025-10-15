#pragma once

#include "Resource.h"
#include "Texture.h"
#include "Graphics/VulkanImage.h"

namespace bl {

// Textures are all QOI format, probably
class Texture2D : public Texture
{ 
public:
    Texture2D(const std::filesystem::path& path);
    ~Texture2D();

    VulkanImage* GetImage();

private:
    VulkanDevice* _device;
    std::unique_ptr<VulkanImage> _image;
};

} // namespace bl