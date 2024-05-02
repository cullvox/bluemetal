#pragma once

#include "AudioSystem.h"

namespace bl
{

class Listener
{
public:
    Listener(AudioSystem* system);
    ~Listener();

    void SetAttributes3D(glm::vec3 position, glm::vec3 velocity, glm::vec3 forward, glm::vec3 up);

private:
    AudioSystem* _system;
};

} // namespace bl