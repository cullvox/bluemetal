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
    AudioListener(std::shared_ptr<AudioSystem> system);
    ~AudioListener();

    void set3DAttributes(Vector3i position, Vector3i velocity);

};

} // namespace bl