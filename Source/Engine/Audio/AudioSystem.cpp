#include "Engine/Engine.h"
#include "AudioSystem.h"

#include "Resources/Sound.h"

namespace bl 
{

AudioSystem::AudioSystem()
{
    FMOD_CHECK(FMOD::System_Create(&_fmod, FMOD_VERSION))
    FMOD_CHECK(_fmod->init(128, FMOD_INIT_NORMAL, nullptr))
}

AudioSystem::~AudioSystem() 
{ 
    _fmod->close();
}

FMOD::System* AudioSystem::GetFMOD()
{ 
    return _fmod; 
}

void AudioSystem::Update()
{
    FMOD_CHECK(_fmod->update())
}

std::string AudioSystem::GetDriverName()
{
    int driverId = 0;
    char name[128] = {0};
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

} // namespace bl