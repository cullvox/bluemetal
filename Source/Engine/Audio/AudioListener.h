#pragma once


///////////////////////////////
// Headers
///////////////////////////////

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

    void set3DAttributes(glm::vec3 position, glm::vec3 velocity);

private:
    AudioSystem* _system;
};

} // namespace bl