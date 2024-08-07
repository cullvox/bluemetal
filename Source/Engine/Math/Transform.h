#pragma once

#include "Math.h"

namespace bl
{

struct Transform 
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 GetMatrix() 
    {
        glm::mat4 mat;
        mat = glm::translate(mat, position);
        mat = glm::scale(mat, scale);
        mat *= glm::mat4(rotation);
        return mat;
    }
};

} // namespace bl