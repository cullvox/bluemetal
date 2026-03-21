#pragma once

#include "Resources/Resource.h"

namespace FMOD
{
    class Sound;
}

namespace bl {

class Sound : public Resource {
    FMOD::Sound* _sound;

public:
    Sound(Engine& engine, const std::filesystem::path& path);
    ~Sound();

    FMOD::Sound* Get();
};

} // namespace bl
