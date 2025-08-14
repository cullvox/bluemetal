#pragma once

#include "Resource.h"
#include "Texture.h"
#include "VulkanImage.h"

namespace bl {

class Texture2D : public Texture
{ 
public:
    Texture2D();
    ~Texture2D();

    virtual bool Load() override;
    virtual void Unload() override;

    VulkanImage* GetImage();

private:
    VulkanDevice* _device;
    std::unique_ptr<VulkanImage> _image;
};

} // namespace bl