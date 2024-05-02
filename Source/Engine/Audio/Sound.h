#pragma once

#include "Resource/Resource.h"
#include "AudioSystem.h"


namespace bl
{

class AudioSystem;

class Sound : public Resource
{
public:
    Sound(AudioSystem* system, const std::filesystem::path& path);
    ~Sound();

    virtual void Load(); /** @brief From Resource, loads the sound file into memory. */
    virtual void Unload(); /** @brief From Resource, frees the sound file from memory. */

    FMOD::Sound* Get();

private:
    AudioSystem* _system;
    std::filesystem::path _path;
    FMOD::Sound* _sound;
};

} // namespace bl