#pragma once

#include "AudioSystem.h"

namespace bl
{

class Listener
{
public:
    Listener(AudioSystem& system);
    ~Listener();

    void SetAttributes3D(glm::vec3 position, glm::vec3 velocity);

private:
    AudioSystem& _system;
};

} // namespace bl