
///////////////////////////////
// Headers
///////////////////////////////

#include "AudioSubsystem.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

AudioSubsystem::AudioSubsystem(Engine& engine)
    : m_engine(engine)
{
}

AudioSubsystem::~AudioSubsystem()
{
}

void AudioSubsystem::init()
{
    m_system = std::make_shared<AudioSystem>();
}

void AudioSubsystem::shutdown()
{
}

void AudioSubsystem::update()
{
    m_system->update();
}

std::shared_ptr<Sound> AudioSubsystem::createSound(std::filesystem::path path)
{
    return std::make_shared<Sound>(m_system, path);
}

std::shared_ptr<AudioListener> AudioSubsystem::createListener()
{
    return std::make_shared<AudioListener>(m_system);
}

std::shared_ptr<AudioSource> AudioSubsystem::createSource()
{
    return std::make_shared<AudioSource>(m_system);
}

}