#pragma once

#include "Precompiled.h"
#include "Core/MacroUtils.h"
#include "Resources/ResourceManager.h"

#include "FMOD.h"

namespace bl
{

class Engine;
class Sound;

class AudioSystem
{
    FMOD::System* _fmod;
public:
    AudioSystem();
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    ~AudioSystem();
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;

    FMOD::System* GetFMOD(); /** @brief Returns the internal FMOD system. */
    void Update(); /** @brief Ticks the audio system. */
    std::string GetDriverName();
    int GetNumChannelsPlaying();
};

} // namespace bl
