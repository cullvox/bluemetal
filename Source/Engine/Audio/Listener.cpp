#include "Listener.h"

namespace bl 
{

Listener::Listener(AudioSystem* pSystem)
    : _system(pSystem)
{
}

Listener::~Listener() 
{
}

void Listener::SetAttributes3D(glm::vec3 p, glm::vec3 v, glm::vec3 f, glm::vec3 u)
{
    FMOD_VECTOR pos{p.x, p.y, p.z};
    FMOD_VECTOR vel{v.x, v.y, v.z};
    FMOD_VECTOR fwd{f.x, f.y, f.z};
    FMOD_VECTOR up{u.x, u.y, u.z};
    FMOD_CHECK(_system->Get()->set3DListenerAttributes(0, &pos, &vel, &fwd, &up))
}

} // namespace bl