#pragma once

#include "Resources/Resource.h"
#include "Audio/AudioSystem.h"

namespace bl
{

class AudioSystem;

class Sound : public Resource
{
    FMOD::Sound* _sound;
public:
    Sound(ResourceSystem* resourceSystem, AudioSystem* system, const std::filesystem::path& path);
    ~Sound();

    FMOD::Sound* Get();
};

} // namespace bl