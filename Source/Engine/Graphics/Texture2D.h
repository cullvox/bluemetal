#pragma once

#include "Resource/Resource.h"
#include "Texture.h"
#include "VulkanImage.h"

namespace bl {

class Texture2D : public Texture { 
public:

    /// @brief Texture Constructor
    Texture2D(const nlohmann::json& data, VulkanDevice* device);
    
    /// @brief Destructor
    ~Texture2D();

    /// @brief Loads the texture into GPU memory.
    virtual void Load() override;

    /// @brief Unloads the texture from GPU memory.
    virtual void Unload() override;

private:
    VulkanDevice* _device;
    VulkanImage _image;
};

} // namespace bl