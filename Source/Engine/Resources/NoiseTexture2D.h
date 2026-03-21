#pragma once

#include "Texture2D.h"

namespace bl {

class NoiseTexture2D : public Texture2D {
public:

    /** @brief Creates a noise texture from a json descriptor file.
     *
     */
    NoiseTexture2D(Engine& engine, const std::filesystem::path& path);

    /** @brief Destructor. */
    ~NoiseTexture2D() = default;

};

} // namespace bl
