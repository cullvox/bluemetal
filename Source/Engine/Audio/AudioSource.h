#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Math/Vector3.h"
#include "Sound.h"
#include "Export.h"


namespace bl
{

///////////////////////////////
// Forward Declarations
///////////////////////////////

class AudioSystem;

///////////////////////////////
// Classes
///////////////////////////////

class BLUEMETAL_API AudioSource
{
public:
    AudioSource(AudioSystem* pSystem);
    ~AudioSource();

    void setSound(Sound* pSound);
    void set3DAttributes(Vector3f position, Vector3f velocity);
    void play(bool repeat = false);

    bool isPlaying();
    bool isStopped();

private:
    AudioSystem*    _system;
    Sound*          m_pSound;
    FMOD_CHANNEL*   m_pChannel;
};

} // namespace bl