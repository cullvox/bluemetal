#include "NoiseTexture2D.h"
#include "Core/Print.h"
#include "Graphics/GraphicsSystem.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <FastNoise/FastNoise.h>

namespace bl {

NoiseTexture2D::NoiseTexture2D(ResourceSystem& rs, GraphicsSystem* gs, const std::filesystem::path& path)
    : Texture2D(rs, gs)
{

    std::ifstream noiseJsonFile { path };
    if (!noiseJsonFile.is_open()) {
        throw std::runtime_error("Could not open noise texture JSON file.");
    }

    FastNoise::SmartNode<> noise;
    auto remap = FastNoise::New<FastNoise::Remap>();

    int width = 0, height = 0;
    int xOffset = 0, yOffset = 0;
    float frequency = 0.0f;
    int seed = 0;
    bool tileable = false;

    try {
        nlohmann::json json = nlohmann::json::parse(noiseJsonFile);

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
    _image = std::make_unique<VulkanImage>(gs->GetDevice(), VK_IMAGE_TYPE_2D, extent, VK_FORMAT_R8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    _image->UploadData(pixels);

}


}