#include "AudioSystem.h"
#include "Resources/Sound.h"
#include "Engine/Engine.h"

namespace bl 
{

AudioSystem::AudioSystem(Engine& _engine)
    : System(_engine)
    , _fmod(nullptr)
{
    GetEngine().GetResourceManager()->AddSystemType<Sound>(this);

    FMOD_CHECK(FMOD::System_Create(&_fmod, FMOD_VERSION))
    FMOD_CHECK(_fmod->init(128, FMOD_INIT_NORMAL, nullptr))
}

AudioSystem::~AudioSystem() 
{ 
    _fmod->close();
}

std::unique_ptr<Resource> AudioSystem::ConstructResource(ResourceSystem* resourceSystem, std::size_t typeHash, const std::filesystem::path& path)
{
    if (typeHash == typeid(Sound).hash_code()) 
    {
        return std::make_unique<Sound>(resourceSystem, this, path);
    }

    throw std::runtime_error("AudioSystem cannot construct resource of the given type!");
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