#pragma once

#include "Sound.h"

namespace bl
{

class AudioSystem;

class Source
{
public:
    Source(AudioSystem* system);
    ~Source();

    void SetSound(Sound* sound);
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