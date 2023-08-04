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
    AudioSource(std::shared_ptr<AudioSystem> device);
    ~AudioSource();

    void setSound(std::shared_ptr<Sound> sound);
    void set3DAttributes(Vector3f position, Vector3f velocity);
    void play(bool repeat = false);

    bool isPlaying();
    bool isStopped();

private:
    std::shared_ptr<AudioSystem>    m_system;
    std::shared_ptr<Sound>          m_sound;
    FMOD_CHANNEL*                   m_pChannel;
};

} // namespace bl