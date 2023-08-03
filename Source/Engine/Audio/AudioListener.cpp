
///////////////////////////////
// Headers
///////////////////////////////

#include "AudioListener.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

AudioListener::AudioListener(std::shared_ptr<AudioSystem> system)
    : m_system(system)
{
}

AudioListener::~AudioListener()
{
}

} // namespace bl