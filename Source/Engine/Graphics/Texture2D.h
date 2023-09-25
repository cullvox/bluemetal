#pragma once

#include "Math/Vector2.h"
#include "Device.h"
#include "Image.h"

namespace bl
{

class Texture2D : public Resource
{
public:
    Texture2D();
    Texture2D(std::shared_ptr<Device> device, TextureFormat format, Extent2D extent, const std::vector<uint8_t>& bytes);
    Texture2D(std::shared_ptr<Device> device, )
    Texture2D(const Texture2D& other);
    Texture2D(Texture2D&& other);
    ~Texture2D();

    void createFromMemory(std::shared_ptr<Device> device, TextureFormat format, Extent2D extent, const std::vector<uint8_t>& bytes);
    

private:
    std::unique_ptr<Image> m_image;
};

} // namespace bl