#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "fmod.h"
#include "AudioSystem.h"

#include "Resource/Resource.h"

namespace bl
{

///////////////////////////////
// Forward Declarations
///////////////////////////////

class AudioSystem;

///////////////////////////////
// Classes
///////////////////////////////

class BLUEMETAL_API Sound
{
public:
    Sound(AudioSystem* pSystem, const std::filesystem::path& file);
    ~Sound();

    FMOD_SOUND* getHandle();

private:
    AudioSystem*    _system;
    FMOD_SOUND*     m_pSound;
};

} // namespace bl