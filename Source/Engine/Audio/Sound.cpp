#include "AudioSystem.h"
#include "Sound.h"

namespace bl 
{

Sound::Sound(AudioSystem* system)
    : _system(system)
{
}

Sound::~Sound() 
{ 
    if (_sound)
        Unload();
}

void Sound::Load(const nlohmann::json&)
{
    FMOD_CHECK(_system->Get()->createSound(GetPath().c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
}

void Sound::Unload()
{
    FMOD_CHECK(_sound->release())
}

FMOD::Sound* Sound::Get() 
{ 
    return _sound; 
}

} // namespace bl