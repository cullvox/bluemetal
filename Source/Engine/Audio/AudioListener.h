#pragma once


///////////////////////////////
// Headers
///////////////////////////////

#include "Math/Vector3.h"
#include "AudioSystem.h"
#include "Export.h"


namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

class BLUEMETAL_API AudioListener
{
public:
    AudioListener(AudioSystem* pSystem);
    ~AudioListener();

    void set3DAttributes(Vector3i position, Vector3i velocity);

private:
    AudioSystem* _system;
};

} // namespace bl