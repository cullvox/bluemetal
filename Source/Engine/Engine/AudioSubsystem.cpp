
///////////////////////////////
// Headers
///////////////////////////////

#include "AudioSubsystem.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

AudioSubsystem::AudioSubsystem(Engine* engine)
    : engine(engine)
{
}

AudioSubsystem::~AudioSubsystem()
{
}

void AudioSubsystem::init()
{
    system = std::make_unique<AudioSystem>();
}

void AudioSubsystem::shutdown()
{
    system.release();
}

void AudioSubsystem::update()
{
    system->update();
}

std::unique_ptr<Sound> AudioSubsystem::createSound(std::filesystem::path path)
{
    return std::move(std::make_unique<Sound>(system.get(), path));
}

std::unique_ptr<AudioListener> AudioSubsystem::createListener()
{
    return std::move(std::make_unique<AudioListener>(system.get()));
}

std::unique_ptr<AudioSource> AudioSubsystem::createSource()
{
    return std::move(std::make_unique<AudioSource>(system.get()));
}

}