#include "Sound.h"

namespace bl {

///////////////////////////////
// Classes
///////////////////////////////

Sound::Sound(AudioSystem* pSystem, const std::filesystem::path& soundPath)
    : m_pSystem(pSystem)
{

    std::string path { soundPath.string() };

    // FMOD_CREATESOUNDEXINFO exInfo = {};
    // exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    // exInfo.length = (unsigned int)path.size();

    FMOD_RESULT result = FMOD_System_CreateSound(
        m_pSystem->getHandle(), path.c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &m_pSound);
    if (result != FMOD_OK) {
        throw std::runtime_error("Could not create an FMOD Sound!");
    }
}

Sound::~Sound() { FMOD_Sound_Release(m_pSound); }

FMOD_SOUND* Sound::getHandle() { return m_pSound; }

}