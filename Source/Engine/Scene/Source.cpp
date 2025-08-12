#include "Audio/AudioSystem.h"
#include "Source.h"

namespace bl 
{

AudioSource3D::AudioSource3D()
{
}

AudioSource3D::~AudioSource3D() 
{
}

void AudioSource3D::Update(float dt)
{
    Node3D::Update(dt);

    if (!IsPlaying()) return;

    // Update the sources 3D attributes.
    glm::vec3 pos = GetGlobalPosition();
    glm::vec3 vel = pos - _prevPosition;
    _prevPosition = pos;

    FMOD_VECTOR fmodPosition{pos.x, pos.y, pos.z};
    FMOD_VECTOR fmodVelocity{vel.x, vel.y, vel.z};

    FMOD_CHECK(_channel->set3DAttributes(&fmodPosition, &fmodVelocity))
}

void AudioSource3D::SetPause(bool paused = false)
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
    AudioSystem* system = AudioSystem::GetInstance();
    FMOD_CHECK(system->GetFMOD()->playSound(sound.Get()->Get(), nullptr, false, &_channel))
}

} // namespace bl