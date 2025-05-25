#pragma once

#include "Resource/Resource.h"
#include "Texture.h"
#include "VulkanImage.h"

namespace bl {

class Texture2D : public Texture 
{ 
public:
    Texture2D(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device);
    ~Texture2D();

    virtual void Load() override;
    virtual void Unload() override;

    VulkanImage* GetImage();

private:
    VulkanDevice* _device;
    VulkanImage _image;
};

} // namespace bl