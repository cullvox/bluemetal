#pragma once

#include "Core/Object.h"
#include "Texture2D.h"

namespace bl {

class NoiseTexture2D : public Texture2D {
    OBJECT_BOILER(NoiseTexture2D, Texture2D)

public:

    NoiseTexture2D();

    NoiseTexture2D(const NoiseTexture2D& noiseTexture);

    /** @brief Creates a noise texture from a json descriptor file.
     *
     */
    NoiseTexture2D(const std::filesystem::path& path);

    /** @brief Destructor. */
    ~NoiseTexture2D() = default;

    virtual void Load() override;
    virtual void Release() override;

    static void RegisterClass();

};

} // namespace bl
