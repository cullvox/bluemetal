#pragma once

#include "Resource/Resource.h"
#include "AudioSystem.h"


namespace bl
{

class AudioSystem;

class Sound : public Resource
{
public:
    Sound(AudioSystem& system, const std::filesystem::path& file);
    ~Sound();

    FMOD::Sound* Get();

private:
    AudioSystem& _system;
    FMOD::Sound* _sound;
};

} // namespace bl