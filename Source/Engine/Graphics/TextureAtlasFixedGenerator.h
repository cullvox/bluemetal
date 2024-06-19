#pragma once

#include "Precompiled.h"
#include "Math/Rect.h"
#include "Resource/Resource.h"
#include "Texture2D.h"

namespace bl {

/// @brief A texture atlas with fixed square textures.
/// A texture atlas with a fixed sprite/subtexture size. The width and height
/// of each subtexture must be power of two. 
class TextureAtlasFixed : public Resource {
public:

    /// @brief Fixed Texture Atlas Constructor
    /// @param[in] maxExtent Max size the width and height parameters of the texture.
    /// @param[in] maxHeight Size of each individual subtexture's width and height.
    TextureAtlasFixedGenerator(uint32_t maxExtent, uint32_t subExtent);
    
    /// @brief Destructor
    ~TextureAtlasFixedGenerator();

    /// @brief Adds a texture to the end of the atlas.
    /// @param[in] texture New texture to add, width and height must be equal to subExtent. 
    uint32_t AddSubTexture(ResourceRef<Texture> texture);

    /// @brief 
    VkRect2D GetSubTextureRect(uint32_t index);
    
    /// @brief Returns the texture resource.
    ResourceRef<Texture2D> GetTexture() const;

private:
    uint32_t _maxExtent;
    uint32_t _subExtent;
    std::vector<Rect<uint32_t>> _subCoordinates;
    VulkanImage _image;
};

} // namespace bl