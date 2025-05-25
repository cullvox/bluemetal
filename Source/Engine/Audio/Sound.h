#pragma once

#include "Resource/Resource.h"

#include "FMOD.h"

namespace bl
{

class AudioSystem;

class Sound : public Resource
{
public:
    Sound(ResourceManager* manager, const nlohmann::json& data, AudioSystem* system);
    ~Sound();

    virtual void Load(); /** @brief From Resource, loads the sound file into memory. */
    virtual void Unload(); /** @brief From Resource, frees the sound file from memory. */

    FMOD::Sound* Get();

private:
    AudioSystem* _system;
    FMOD::Sound* _sound;
};

} // namespace bl