#pragma once

#include "Core/Object.h"
#include "Resources/Resource.h"

namespace FMOD
{
    class Sound;
}

namespace bl {

class Sound : public Resource {
    OBJECT_BOILER(Sound, Resource)
    FMOD::Sound* _sound;

public:
    Sound();
    Sound(const std::filesystem::path& path);
    Sound(const Sound& sound);
    ~Sound();

    static void RegisterClass();
    virtual void Load() override;
    virtual void Release() override;

    FMOD::Sound* Get();
};

} // namespace bl
