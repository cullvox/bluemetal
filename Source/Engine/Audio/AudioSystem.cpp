#include "AudioSystem.h"

namespace bl {

AudioSystem::AudioSystem()
{
    if (FMOD_System_Create(&_system, FMOD_VERSION) != FMOD_OK) {
        throw std::runtime_error("Could not create an FMOD System!");
    }

    if (FMOD_System_Init(_system, 128, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) {
        throw std::runtime_error("Could not init FMOD System!");
    }
}

AudioSystem::~AudioSystem() { FMOD_System_Close(_system); }

FMOD_SYSTEM* AudioSystem::get() { return _system; }

std::string AudioSystem::getDriverName()
{
    int driver = 0;
    FMOD_System_GetDriver(_system, &driver);

    char name[32] = {0};
    FMOD_System_GetDriverInfo(_system, driver, name, sizeof(name), nullptr, nullptr, nullptr, nullptr);

    return std::string(name);
}

int AudioSystem::getNumChannelsPlaying()
{
    int count = 0;
    FMOD_System_GetChannelsPlaying(_system, &count, nullptr);
    return count;
}

void AudioSystem::update()
{
    if (FMOD_System_Update(_system) != FMOD_OK) {
        throw std::runtime_error("Could not update FMOD System!");
    }
}

} // namespace bl