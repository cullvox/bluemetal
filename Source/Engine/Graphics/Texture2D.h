#pragma once

#include "Resource/Resource.h"
#include "Texture.h"
#include "VulkanImage.h"

namespace bl {

class Texture2D : public Texture { 
public:
    Texture2D(const nlohmann::json& data);
    ~Texture2D();

    virtual void Load() override;
    virtual void Unload() override;

private:
    VulkanImage _image;
};

} // namespace bl