#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "fmod.h"
#include "Precompiled.h"
#include "Export.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

class BLUEMETAL_API AudioSystem
{
public:
    AudioSystem();
    ~AudioSystem();

    FMOD_SYSTEM* getHandle();
    void update();

private:
    FMOD_SYSTEM* m_pSystem;
    
};

} // namespace bl