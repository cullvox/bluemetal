#include "AudioSystem.h"
#include "Sound.h"

namespace bl 
{

Sound::Sound(ResourceManager* manager, const nlohmann::json& json, AudioSystem* system)
    : Resource(manager, json)
    , _system(system)
{
}

Sound::~Sound() 
{ 
    if (_sound)
        Unload();
}

void Sound::Load()
{
    FMOD_CHECK(_system->Get()->createSound(GetPath().string().c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
}

void Sound::Unload()
{
    FMOD_CHECK(_sound->release())
    _sound = nullptr;
}

FMOD::Sound* Sound::Get() 
{ 
    return _sound; 
}

} // namespace bl