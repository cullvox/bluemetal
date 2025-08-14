#include "Sound.h"
#include "Audio/AudioSystem.h"

namespace bl {

Sound::Sound()
    : _sound(nullptr)
{
}

Sound::~Sound()
{
    if (_sound)
        Unload();
}

bool Sound::Load()
{
    FMOD_CHECK(_system->GetFMOD()->createSound(GetPath().c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
    return true;
}

void Sound::Unload()
{
    FMOD_CHECK(_sound->release())
    _sound = nullptr;
}

bool Sound::ExportBinary(std::ostream&) const
{
    return false;
}

FMOD::Sound* Sound::Get()
{
    return _sound;
}

} // namespace bl