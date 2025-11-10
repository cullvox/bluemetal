#pragma once

#include "Precompiled.h"
#include "Texture.h"

namespace bl {

class GraphicsSystem;

// Textures are all QOI format, probably
class Texture2D : public Texture
{
    VulkanDevice* _device;
    bool DecodeQOI(const std::vector<std::byte>& data, std::vector<std::byte>& out);
    bool DecodeSTBI(const std::vector<std::byte>& data, std::vector<std::byte>& out);
public:
    Texture2D(ResourceSystem* rs, GraphicsSystem* gs, const std::filesystem::path& path);
    Texture2D(ResourceSystem* rs, GraphicsSystem* gs, std::span<std::byte> data);
    ~Texture2D() = default;
};

} // namespace bl