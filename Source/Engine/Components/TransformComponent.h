#pragma once

#include "Precompiled.h"

namespace bl
{

class TransformComponent
{
public:
    TransformComponent() = default;
    ~TransformComponent() = default;

    glm::vec3 GetPosition();
    glm::vec3 GetEulerRotation();
    glm::quat GetRotation();
    glm::vec3 GetScale();
    void SetPosition(glm::vec3 position);
    void SetEulerRotation(glm::vec3 rotation);
    void SetRotation(glm::quat rotation);
    void SetScale(glm::vec3 scale);
    glm::mat4 GetMatrix();

private:
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
};

} // namespace bl