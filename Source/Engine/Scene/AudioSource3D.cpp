#include "AudioSource3D.h"
#include "Audio/FMOD.h"
#include "Audio/AudioSystem.h"
#include "Engine/Engine.h"
#include "Core/ClassDB.h"

namespace bl {

AudioSource3D::AudioSource3D(Engine& engine)
    : Node3D(engine)
{
    _system = GetEngine().GetAudio();
}

AudioSource3D::~AudioSource3D()
{
}

void AudioSource3D::Update(float dt)
{
    Node3D::Update(dt);

    if (!IsPlaying())
        return;

    // Update the sources 3D attributes.
    glm::vec3 pos = GetWorldPosition();
    glm::vec3 vel = pos - _prevPosition;
    _prevPosition = pos;

    FMOD_VECTOR fmodPosition { pos.x, pos.y, pos.z };
    FMOD_VECTOR fmodVelocity { vel.x, vel.y, vel.z };

    FMOD_CHECK(_channel->set3DAttributes(&fmodPosition, &fmodVelocity))
}

void AudioSource3D::SetPause(bool paused)
{
    FMOD_CHECK(_channel->setPaused(paused))
}

bool AudioSource3D::IsPlaying()
{
    bool playing = false;
    FMOD_CHECK(_channel->isPlaying(&playing))
    return playing;
}

void AudioSource3D::Play(Ref<Sound> sound, bool repeat)
{
    FMOD_CHECK(_system->GetFMOD()->playSound(sound.lock()->Get(), nullptr, false, &_channel))
    FMOD_CHECK(_channel->setLoopCount(repeat ? -1 : 0))
}

void AudioSource3D::SetVolume(float volume)
{
    FMOD_CHECK(_channel->setVolume(volume))
}

void AudioSource3D::RegisterClass(ClassDB& db)
{
    db.RegisterClass("AudioSource3D", &AudioSource3D::Create);
}

} // namespace bl