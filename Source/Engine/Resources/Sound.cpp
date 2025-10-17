#include "Sound.h"

namespace bl {

Sound::Sound(ResourceSystem* resourceSystem, AudioSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
    , _sound(nullptr)
{
    FMOD_CHECK(system->GetFMOD()->createSound(path.string().c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
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