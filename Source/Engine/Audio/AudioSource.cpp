#include "AudioSource.h"

namespace bl
{

AudioSource::AudioSource(std::shared_ptr<AudioSystem> device)
    : m_device(device)
{    
}

} // namespace bl