#include "Sound.h"
#include "Engine/Engine.h"
#include "Audio/AudioSystem.h"

namespace bl {

Sound::Sound(const std::filesystem::path& path)
    : _sound(nullptr)
{
    auto system = GetEngine()->GetAudio();
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