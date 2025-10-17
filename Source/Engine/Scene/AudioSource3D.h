#pragma once

#include "Node3D.h"
#include "Math/Math.h"
#include "Resources/Sound.h"

namespace bl
{

class AudioSystem;

class AudioSource3D : public Node3D
{
    AudioSystem* _system;
    Ref<Sound> _sound;
    glm::vec3 _prevPosition; /** @brief Used for calculating the node's velocity. */
    FMOD::Channel* _channel;
public:
    AudioSource3D(Engine* engine);
    ~AudioSource3D();

    virtual void Update(float dt) override;

    void SetPause(bool paused = false);
    void Play(Ref<Sound> sound, bool repeat = false);

    bool IsPlaying();
    bool IsStopped();
};

} // namespace bl