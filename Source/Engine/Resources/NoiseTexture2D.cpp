#include "NoiseTexture2D.h"
#include "Core/ClassDB.h"
#include "Core/Print.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Resources/Texture.h"

namespace bl {

NoiseTexture2D::NoiseTexture2D()
{
}

NoiseTexture2D::NoiseTexture2D(const NoiseTexture2D& copy)
{
    throw std::runtime_error("Copy not implemented");
}

NoiseTexture2D::NoiseTexture2D(const std::filesystem::path& path)
    : Texture2D()
{

    

}

void NoiseTexture2D::Load()
{
  
    const auto& json = GetJson();

    FastNoise::SmartNode<> noise;
    auto remap = FastNoise::New<FastNoise::Remap>();

    int width = 0, height = 0;
    int xOffset = 0, yOffset = 0;
    float frequency = 0.0f;
    int seed = 0;
    bool tileable = false;

    try {
        width = json.value("width", 64);
        height = json.value("height", 64);
        xOffset = json.value("xOffset", 0);
        yOffset = json.value("yOffset", 0);
        frequency = json.value("frequency", 1.0f);
        seed = json.value("seed", 0);
        tileable = json.value("tileable", false);

        if (width < 0 || height < 0) {
            Print::Error("Invalid width, height value for noise texture.");
            return;
        }

        if (width > 4096 || height > 4096) {
            Print::Warn("Large noise texture.");
        }

        std::string noiseType = json.value("noiseType", "white");

        if (noiseType == "white") {
            noise = FastNoise::New<FastNoise::White>();
            remap->SetSource(noise);
            remap->SetRemap(-1.0f, 1.0f, 0.0f, 255.0f);
        }

        if (noiseType == "perlin") {
            noise = FastNoise::New<FastNoise::Perlin>();
            remap->SetSource(noise);
            remap->SetRemap(-1.0f, 1.0f, 0.0f, 255.0f);
        }

        if (noiseType == "simplex") {
            auto simplex = FastNoise::New<FastNoise::Simplex>();
            remap->SetSource(noise);
            remap->SetRemap(-1.0f, 1.0f, 0.0f, 255.0f);
        }


    } 
    catch (const nlohmann::json::exception& e)
    {
        Print::Error("Could not parse noise texture json file! Error: {}", e.what());
    }

    std::vector<float> values(width * height);

    if (tileable) {
        remap->GenTileable2D(values.data(), width, height, frequency, seed);
    } else {
        remap->GenUniformGrid2D(values.data(), xOffset, yOffset, width, height, frequency, seed);
    }

    std::vector<std::byte> pixels(width * height);
    for (size_t i = 0; i < pixels.size(); i++) {
        pixels[i] = static_cast<std::byte>(values[i]);
    }

    // Create texture.
    _extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    VkExtent3D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
    _image = std::make_unique<VulkanImage>(GraphicsSystem::Get()->GetDevice(), VK_IMAGE_TYPE_2D, extent, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    _image->UploadData(pixels);
}
void NoiseTexture2D::Release()
{
    Texture2D::Release();
    _image.reset();
}

void NoiseTexture2D::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("NoiseTexture2D", "Texture2D", &NoiseTexture2D::Create);
}


}