#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "fmod.h"

namespace bl
{


///////////////////////////////
// Classes
///////////////////////////////

class AudioChannel
{
public:
    AudioChannel(std::shared_ptr<AudioSystem> device)
    ~AudioChannel();

private:
    FMOD_CHANNEL* m_pChannel;
};

} // namespace bl