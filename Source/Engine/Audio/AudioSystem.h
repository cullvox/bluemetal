#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "fmod.h"
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

    void update();

private:
    FMOD_SYSTEM* m_pSystem;
    
};

} // namespace bl