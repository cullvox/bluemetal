#pragma once

#include "Precompiled.h"
#include "Texture.h"
#include "VulkanImage.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>

namespace bl {

/// @brief A texture atlas with fixed square textures.
/// A texture atlas with a fixed sprite/subtexture size. The width and height
/// of each subtexture must be power of two. 
class TextureAtlasFixed {
public:

    /// @brief Fixed Texture Atlas Constructor
    /// @param[in] maxExtent Max size the width and height parameters of the texture.
    /// @param[in] maxHeight Size of each individual subtexture's width and height.
    TextureAtlasFixed(uint32_t maxExtent, uint32_t subExtent);
    
    /// @brief Destructor
    ~TextureAtlasFixed();

    /// @brief Adds a texture to the end of the atlas.
    /// @param[in] texture New texture to add, width and height must be equal to subExtent. 
    uint32_t AddSubTexture(ResourceRef<Texture> texture);

    /// @brief 
    VkRect2D GetSubTextureRect(uint32_t index);

private:
    struct SubTexture {
        uint32_t start;
        uint32_t width;
    };

    uint32_t maxExtent;
    uint32_t subExtent;
    bl::VulkanImage
};

} // namespace bl