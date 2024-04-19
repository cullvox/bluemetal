#pragma once

#include "Precompiled.h"

namespace bl
{

struct Transform 
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 getMatrix() 
    {
        return glm::identity<glm::mat4>();
    }
};

} // namespace bl