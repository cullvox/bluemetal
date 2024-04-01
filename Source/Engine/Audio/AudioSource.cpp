#include "AudioSource.h"

#include "fmod.h"

namespace bl {

AudioSource::AudioSource(AudioSystem* pSystem)
    : _system(pSystem)
{
}

AudioSource::~AudioSource() { }

void AudioSource::setSound(Sound* pSound) { m_pSound = pSound; }

void AudioSource::set3DAttributes(Vector3f position, Vector3f velocity)
{
    FMOD_VECTOR pos { position.x, position.y, position.z };
    FMOD_VECTOR vel { velocity.x, velocity.y, velocity.z };
    FMOD_Channel_Set3DAttributes(m_pChannel, &pos, &vel);
}

void AudioSource::play(bool repeat)
{
    if (FMOD_System_PlaySound(
            _system->get(), m_pSound->getHandle(), nullptr, false, &m_pChannel)
        != FMOD_OK) {
        throw std::runtime_error("Could not play an FMOD Sound!");
    }
}

} // namespace bl