#include "Sound.h"

namespace bl 
{

Sound::Sound(AudioSystem& system, const std::filesystem::path& soundPath)
    : _system(system)
{
    FMOD_CHECK(_system.Get()->createSound(soundPath.c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
}

Sound::~Sound() 
{ 
    _sound->release();
}

FMOD::Sound* Sound::Get() 
{ 
    return _sound; 
}

} // namespace bl