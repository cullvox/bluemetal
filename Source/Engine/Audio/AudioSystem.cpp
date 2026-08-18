#include "AudioSystem.h"
#include "Engine/Engine.h"
#include "Resources/Sound.h"

#include "Resources/ResourceSystem.h"

namespace bl {

AudioSystem::AudioSystem()
    : System()
    , _fmod(nullptr)
{
    // Initialize FMOD
    FMOD_CHECK(FMOD::System_Create(&_fmod, FMOD_VERSION))
    FMOD_CHECK(_fmod->init(128, FMOD_INIT_NORMAL, nullptr))

    // Create all of our sound bus/groups
    FMOD_CHECK(_fmod->createChannelGroup("Master", &_masterGroup));
    FMOD_CHECK(_fmod->createChannelGroup("Music", &_musicGroup));
    FMOD_CHECK(_masterGroup->addGroup(_musicGroup));
}

AudioSystem::~AudioSystem()
{
    // Deinitialize FMOD
    _fmod->close();
}

AudioSystem* AudioSystem::Get()
{
    static AudioSystem system;
    return &system;
}

FMOD::System* AudioSystem::GetFMOD()
{
    return _fmod;
}

void AudioSystem::Update() {
    FMOD_CHECK(_fmod->update())
}

std::string AudioSystem::GetDriverName()
{
    int driverId = 0;
    char name[128] = { 0 };
    FMOD_CHECK(_fmod->getDriver(&driverId))
    FMOD_CHECK(_fmod->getDriverInfo(driverId, name, sizeof(name), nullptr, nullptr, nullptr, nullptr))

    return std::string(name);
}

int AudioSystem::GetNumChannelsPlaying()
{
    int count = 0;
    FMOD_CHECK(_fmod->getChannelsPlaying(&count))
    return count;
}

void AudioSystem::SetBusVolume(AudioBus bus, float volume)
{
    FMOD::ChannelGroup* group = GetBusChannelGroup(bus);
    FMOD_CHECK(group->setVolume(volume));
}

FMOD::ChannelGroup* AudioSystem::GetBusChannelGroup(AudioBus bus)
{
    switch (bus) {
        case AudioBus::eMaster:
            return _masterGroup;
        case AudioBus::eMusic:
            return _musicGroup;
        default:
            throw std::runtime_error("Invalid bus!");
    }
}

} // namespace bl