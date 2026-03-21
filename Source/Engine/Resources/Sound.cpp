#include "Sound.h"
#include "Engine/Engine.h"
#include "Audio/AudioSystem.h"

namespace bl {

Sound::Sound(Engine& engine, const std::filesystem::path& path)
    : Resource(engine, path)
    , _sound(nullptr)
{
    FMOD_CHECK(engine.GetAudio()->GetFMOD()->createSound(path.string().c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
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
