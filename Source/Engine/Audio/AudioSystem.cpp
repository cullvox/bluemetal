#include "AudioSystem.h"

namespace bl
{

AudioSystem::AudioSystem()
{
    if (FMOD_System_Create(&m_pSystem, FMOD_VERSION) != FMOD_OK)
    {
        throw std::runtime_error("Could not create an FMOD System!");
    }

    if (FMOD_System_Init(m_pSystem, 128, FMOD_INIT_NORMAL, nullptr) != FMOD_OK)
    {
        throw std::runtime_error("Could not init FMOD System!");
    }
}

AudioSystem::~AudioSystem()
{
    FMOD_System_Close(m_pSystem);
}

FMOD_SYSTEM* AudioSystem::getHandle()
{
    return m_pSystem;
}

void AudioSystem::update()
{
    if (FMOD_System_Update(m_pSystem) != FMOD_OK)
    {
        throw std::runtime_error("Could not update FMOD System!");
    }
}

} // namespace bl