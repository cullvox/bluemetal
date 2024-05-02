#include "Sound.h"

namespace bl 
{

Sound::Sound(AudioSystem* system, const std::filesystem::path& path)
    : _system(system)
    , _path(path)
{
    Load();
}

Sound::~Sound() 
{ 
    Unload();
}

void Sound::Load()
{
    FMOD_CHECK(_system->Get()->createSound(_path.string().c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
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