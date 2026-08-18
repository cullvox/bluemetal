#include "Sound.h"
#include "Engine/Engine.h"
#include "Audio/AudioSystem.h"

namespace bl {

Sound::Sound()
{
}

Sound::Sound(const std::filesystem::path& path)
    : Resource(path)
    , _sound(nullptr)
{
}

Sound::Sound(const Sound& sound)
{
    throw std::runtime_error("Cannot clone a sound yet.");
}

Sound::~Sound()
{
    // _sound->release();
}

void Sound::Load()
{
    const auto& json = GetJson();
    auto path = json["path"].get<std::string>();
    FMOD_CHECK(AudioSystem::Get()->GetFMOD()->createSound(path.c_str(), FMOD_DEFAULT | FMOD_3D, nullptr, &_sound))
}

void Sound::Release()
{
    Resource::Release();
    _sound->release();
}

FMOD::Sound* Sound::Get()
{
    return _sound;
}

void Sound::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Sound", "Resource", &Sound::Create);
}

} // namespace bl
