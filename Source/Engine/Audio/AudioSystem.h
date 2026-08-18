#pragma once

#include <string>

#include "Core/MacroUtils.h"
#include "Engine/System.h"
#include "FMOD.h"
#include "AudioBus.h"

namespace bl {

class Sound;

class AudioSystem : public System {
    FMOD::System* _fmod;
    FMOD::ChannelGroup* _masterGroup;
    FMOD::ChannelGroup* _musicGroup;


    AudioSystem();
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem(AudioSystem&&) = delete;
    ~AudioSystem();
    AudioSystem& operator=(const AudioSystem&) = delete;
    AudioSystem& operator=(AudioSystem&&) = delete;

public:
    static AudioSystem* Get();

    void SetBusVolume(AudioBus bus, float volume);
    FMOD::ChannelGroup* GetBusChannelGroup(AudioBus bus);

    FMOD::System* GetFMOD(); /** @brief Returns the internal FMOD system. */
    void Update(); /** @brief Ticks the audio system. */
    std::string GetDriverName();
    int GetNumChannelsPlaying();
};

} // namespace bl
