
///////////////////////////////
// Headers
///////////////////////////////

#include "AudioSubsystem.h"

namespace bl {

///////////////////////////////
// Classes
///////////////////////////////

AudioSubsystem::AudioSubsystem(Engine& engine)
    : _engine(engine)
{
    _system = std::make_shared<AudioSystem>();
}

AudioSubsystem::~AudioSubsystem() { }

void AudioSubsystem::update() { _system->update(); }

std::shared_ptr<AudioSystem> AudioSubsystem::getSystem() { return _system; }

std::unique_ptr<Sound> AudioSubsystem::createSound(std::filesystem::path path)
{
    return std::make_unique<Sound>(_system.get(), path);
}

std::unique_ptr<AudioListener> AudioSubsystem::createListener()
{
    return std::make_unique<AudioListener>(_system.get());
}

std::unique_ptr<AudioSource> AudioSubsystem::createSource()
{
    return std::make_unique<AudioSource>(_system.get());
}

}