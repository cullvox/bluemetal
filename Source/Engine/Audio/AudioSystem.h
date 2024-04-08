#pragma once

#include "fmod.h"
#include "Precompiled.h"
#include "Export.h"

namespace bl
{

class BLUEMETAL_API AudioSystem
{
public:
    AudioSystem();
    ~AudioSystem();

    FMOD_SYSTEM* get();
    
    std::string getDriverName();
    int getNumChannelsPlaying();

    void update();

private:
    FMOD_SYSTEM* _system;
    
};

} // namespace bl