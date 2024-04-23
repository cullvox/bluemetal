#pragma once

#include "Sound.h"
#include "Export.h"


namespace bl
{

class AudioSystem;

class BLUEMETAL_API AudioSource
{
public:
    AudioSource(AudioSystem* pSystem);
    ~AudioSource();

    void setSound(Sound* pSound);
    void set3DAttributes(glm::vec3 position, glm::vec3 velocity);
    void play(bool repeat = false);

    bool isPlaying();
    bool isStopped();

private:
    AudioSystem*    _system;
    Sound*          m_pSound;
    FMOD_CHANNEL*   m_pChannel;
};

} // namespace bl