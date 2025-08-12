#pragma once

#include "Resources/Resource.h"

#include "FMOD.h"

namespace bl
{

class AudioSystem;

class Sound : public Resource
{
public:
    virtual const std::string& GetType() const override { return "Audio"; }

    Sound(ResourceManager* manager, AudioSystem* system);
    ~Sound();

    virtual bool Load() override; /** @brief From Resource, loads the sound file into memory. */
    virtual void Unload() override; /** @brief From Resource, frees the sound file from memory. */
    virtual bool ExportBinary(std::ostream& stream) const override;

    FMOD::Sound* Get();

private:
    AudioSystem* _system;
    FMOD::Sound* _sound;
};

} // namespace bl