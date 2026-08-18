#pragma once

#include "Math/Math.h"
#include "Node3D.h"
#include "Resources/Sound.h"
#include "Audio/AudioBus.h"

namespace FMOD {
    class Channel;
}

namespace bl {

class AudioSystem;

class AudioSource3D : public Node3D {
    OBJECT_BOILER(AudioSource3D, Node3D)

    Ref<Sound> _sound;
    AudioBus _bus;
    glm::vec3 _prevPosition; /** @brief Used for calculating the node's velocity. */
    FMOD::Channel* _channel;

public:
    AudioSource3D();
    ~AudioSource3D();

    virtual void Update(float dt) override;

    void SetBus(AudioBus bus);
    void SetPause(bool paused = false);
    void Play(Ref<Sound> sound, bool repeat = false);
    void SetVolume(float volume);

    bool IsPlaying();
    bool IsStopped();

    static void RegisterClass();
};

} // namespace bl