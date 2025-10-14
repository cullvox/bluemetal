#pragma once

#include "Resources/Resource.h"

#include "Audio/FMOD.h"

namespace bl
{

class AudioSystem;

class Sound : public Resource
{
    FMOD::Sound* _sound;
public:
    Sound(const std::filesystem::path& path);
    ~Sound();

    FMOD::Sound* Get();
};

} // namespace bl