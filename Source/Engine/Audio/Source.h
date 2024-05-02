#pragma once

#include "Sound.h"
#include "Export.h"


namespace bl
{

class AudioSystem;

class Source
{
public:
    Source(AudioSystem* pSystem);
    ~Source();

    void SetSound(Sound* pSound);
    void Set3DAttributes(glm::vec3 position, glm::vec3 velocity);
    void Play(bool repeat = false);

    bool IsPlaying();
    bool IsStopped();

private:
    AudioSystem* _system;
    Sound* _sound;
    FMOD::Channel* _channel;
};

} // namespace bl