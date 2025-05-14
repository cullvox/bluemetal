#include "AudioSystem.h"
#include "Source.h"

namespace bl 
{

Source::Source(AudioSystem* pSystem)
    : _system(pSystem)
{
}

Source::~Source() 
{
}

void Source::SetSound(ResourceRef<Sound> sound) 
{ 
    _sound = sound;
}

void Source::Set3DAttributes(glm::vec3 position, glm::vec3 velocity)
{
    FMOD_VECTOR pos{position.x, position.y, position.z};
    FMOD_VECTOR vel{velocity.x, velocity.y, velocity.z};

    _channel->set3DAttributes(&pos, &vel);
}

void Source::Play(bool repeat)
{
    _system->Get()->playSound(_sound.Get()->Get(), nullptr, false, &_channel);
}

} // namespace bl