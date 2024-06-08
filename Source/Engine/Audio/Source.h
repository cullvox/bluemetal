#pragma once

#include "Math/Math.h"
#include "Sound.h"

namespace bl {

class AudioSystem;

class Source {
public:
    Source(AudioSystem* system);
    ~Source();

    void SetSound(ResourceRef<Sound> sound);
    void Set3DAttributes(glm::vec3 position, glm::vec3 velocity);
    void Play(bool repeat = false);

    bool IsPlaying();
    bool IsStopped();

private:
    AudioSystem* _system;
    ResourceRef<Sound> _sound;
    FMOD::Channel* _channel;
};

} // namespace bl