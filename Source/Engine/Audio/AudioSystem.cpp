#include "Engine/Engine.h"
#include "AudioSystem.h"
#include "Sound.h"

namespace bl 
{

AudioSystem::AudioSystem(Engine& engine)
    : _engine(engine)
{
    FMOD_CHECK(FMOD::System_Create(&_system, FMOD_VERSION))
    FMOD_CHECK(_system->init(128, FMOD_INIT_NORMAL, nullptr))
}

AudioSystem::~AudioSystem() 
{ 
    _system->close(); 
}

FMOD::System* AudioSystem::Get() 
{ 
    return _system; 
}

void AudioSystem::Update()
{
    FMOD_CHECK(_system->update())
}

std::string AudioSystem::GetDriverName()
{
    int driverId = 0;
    char name[128] = {0};
    FMOD_CHECK(_system->getDriver(&driverId))
    FMOD_CHECK(_system->getDriverInfo(driverId, name, sizeof(name), nullptr, nullptr, nullptr, nullptr))

    return std::string(name);
}

int AudioSystem::GetNumChannelsPlaying()
{
    int count = 0;
    FMOD_CHECK(_system->getChannelsPlaying(&count))
    return count;
}

std::unique_ptr<Resource> AudioSystem::BuildResource(const std::string& type, const std::filesystem::path& path)
{
    if (type == "sound")
    {
        return std::make_unique<Sound>(this, path);
    }
    else
    {
        throw std::runtime_error("Trying to create a resource this builder wasn't specified to!");
    }
}

ResourceRef<Sound> AudioSystem::CreateSound(std::filesystem::path path)
{
    // return  
}

} // namespace bl