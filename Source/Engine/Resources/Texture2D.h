#pragma once

#include "Resource.h"
#include "Texture.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/GraphicsSystem.h"

namespace bl {

class GraphicsSystem;

// Textures are all QOI format, probably
class Texture2D : public Texture
{ 
public:
    Texture2D(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path);
    ~Texture2D();

private:
    VulkanDevice* _device;
};

} // namespace bl